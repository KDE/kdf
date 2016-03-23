/*
 * disklist.cpp
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *               2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "disklist.h"

#include "kdf_debug.h"
#include "kdfutil.h"

#include <kglobal.h>
#include <kconfiggroup.h>
#include <kdefakes.h>
#include <kprocess.h>
#include <klocale.h>

#include <QTextStream>
#include <QFile>
#include <QRegExp>

#include <math.h>
#include <stdlib.h>

static const QLatin1Char Blank = QLatin1Char( ' ' );
static const QLatin1Char Delimiter = QLatin1Char( '#' );

/***************************************************************************
  * constructor
**/
DiskList::DiskList(QObject *parent)
        : QObject(parent), dfProc(new KProcess(this))
{
    qCDebug(KDF);

    updatesDisabled = false;

    if (No_FS_Type)
    {
        qCDebug(KDF) << "df gives no FS_TYPE";
    }

    disks = new Disks();

    // BackgroundProcesses ****************************************
    dfProc->setOutputChannelMode(KProcess::MergedChannels);
    connect(dfProc,SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(dfDone()) );

    readingDFStdErrOut=false;
    config = KGlobal::config();
    loadSettings();
}


/***************************************************************************
  * destructor
**/
DiskList::~DiskList()
{
    dfProc->disconnect();
    if( dfProc->state() == QProcess::Running )
    {
        dfProc->terminate();
        dfProc->waitForFinished();
    }
    delete dfProc;
    //We have to delete the diskentries manually, otherwise they get leaked (?)
    // (they aren't released on delete disks )
    DisksIterator itr = disksIteratorBegin();
    DisksIterator end = disksIteratorEnd();
    while( itr != end )
    {
        DisksIterator prev = itr;
        ++itr;

        DiskEntry * disk = *prev;
        disks->erase( prev );
        delete disk;
    }
    delete disks;
}

/**
Updated need to be disabled sometimes to avoid pulling the DiskEntry out from the popupmenu handler
*/
void DiskList::setUpdatesDisabled(bool disable)
{
    updatesDisabled = disable;
}

/***************************************************************************
  * saves the KConfig for special mount/umount scripts
**/
void DiskList::applySettings()
{
    qCDebug(KDF);

    KConfigGroup group(config, "DiskList");
    QString key;

    DisksConstIterator itr = disksConstIteratorBegin();
    DisksConstIterator end = disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;

        key = QLatin1String("Mount") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        group.writePathEntry(key,disk->mountCommand());

        key = QLatin1String("Umount") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        group.writePathEntry(key,disk->umountCommand());

        key = QLatin1String("Icon") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        group.writePathEntry(key,disk->realIconName());
    }
    group.sync();
}


/***************************************************************************
  * reads the KConfig for special mount/umount scripts
**/
void DiskList::loadSettings()
{
    qCDebug(KDF);

    const KConfigGroup group(config, "DiskList");
    QString key;

    DisksConstIterator itr = disksConstIteratorBegin();
    DisksConstIterator end = disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;

        key = QLatin1String("Mount") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        disk->setMountCommand(group.readPathEntry(key, QString()));

        key = QLatin1String("Umount") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        disk->setUmountCommand(group.readPathEntry(key, QString()));

        key = QLatin1String("Icon") + Separator + disk->deviceName() + Separator + disk->mountPoint();
        QString icon=group.readPathEntry(key, QString());
        if (!icon.isEmpty())
            disk->setIconName(icon);
    }
}


static QString expandEscapes(const QString& s) {
    QString rc;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == QLatin1Char( '\\' ))
        {
            i++;
            QChar str=s.at(i);
            if( str == QLatin1Char( '\\' ))
                rc += QLatin1Char( '\\' );
            else if( str == QLatin1Char( '0' ))
            {
                rc += QLatin1Char( s.mid(i,3).toULongLong(0, 8) );
                i += 2;
            }
            else
            {
                // give up and not process anything else because I'm too lazy
                // to implement other escapes
                rc += QLatin1Char( '\\' );
                rc += s[i];
            }
        }
        else
        {
            rc += s[i];
        }
    }
    return rc;
}

/***************************************************************************
  * tries to figure out the possibly mounted fs
**/
int DiskList::readFSTAB()
{
    qCDebug(KDF);

    if (readingDFStdErrOut || (dfProc->state() != QProcess::NotRunning))
        return -1;

    QFile f(FSTAB);
    if ( f.open(QIODevice::ReadOnly) )
    {
        QTextStream t (&f);
        QString s;
        DiskEntry *disk;

        //disks->clear(); // ############

        while (! t.atEnd())
        {
            s=t.readLine();
            s=s.simplified();

	    if ( (!s.isEmpty() ) && (s.indexOf(Delimiter)!=0) )
	    {
                // not empty or commented out by '#'
                qCDebug(KDF) << "GOT: [" << s << "]";
                disk = new DiskEntry();
                disk->setMounted(false);
		QFile path(QLatin1String( "/dev/disk/by-uuid/" ));
		// We need to remove UUID=
		// TODO: Fix for other OS if using UUID and not using /dev/disk/by-uuid/
		if ( s.contains(QLatin1String( "UUID=" )) )
		{
			if (path.exists())
			{
				QRegExp uuid( QLatin1String( "UUID=(\\S+)(\\s+)" ));
				QString extracted ;
				if (uuid.indexIn(s) != -1)
				{
					extracted = uuid.cap(1);
				}

				if (! extracted.isEmpty() )
				{
					QString device = path.fileName() + extracted;
					QFile file(device);

					if ( file.exists() )
					{
						QString filesym = file.symLinkTarget();
						disk->setDeviceName(filesym);
					}
					else
					{
						qCDebug(KDF) << "The device does not seems to exist";
						continue;
					}
				}
				else
				{
					qCDebug(KDF) << "Invalid UUID";
					continue;
				}
			}
			else
			{
				qCDebug(KDF) << "UUID OK but there is no /dev/disk/by-uuid/";
				continue;
			}
		}
		else
		{
			disk->setDeviceName(expandEscapes(s.left(s.indexOf(Blank))));
		}

		s=s.remove(0,s.indexOf(Blank)+1 );
#ifdef _OS_SOLARIS_
                //device to fsck
                s=s.remove(0,s.indexOf(Blank)+1 );
#endif
		disk->setMountPoint(expandEscapes(s.left(s.indexOf(Blank))));
		s=s.remove(0,s.indexOf(Blank)+1 );
		disk->setFsType(s.left(s.indexOf(Blank)) );
		s=s.remove(0,s.indexOf(Blank)+1 );
                disk->setMountOptions(s.left(s.indexOf(Blank)) );
                s=s.remove(0,s.indexOf(Blank)+1 );

                if ( (disk->deviceName() != QLatin1String( "none" ))
                        && (disk->fsType() != QLatin1String( "swap" ))
                        && (disk->fsType() != QLatin1String( "sysfs" ))
                        && (disk->fsType() != QLatin1String( "rootfs" ))
                        && (disk->fsType() != QLatin1String( "tmpfs" ))
                        && (disk->fsType() != QLatin1String( "debugfs" ))
                        && (disk->fsType() != QLatin1String( "devtmpfs" ))
                        && (disk->mountPoint() != QLatin1String( "/dev/swap" ))
                        && (disk->mountPoint() != QLatin1String( "/dev/pts" ))
                        && (disk->mountPoint() != QLatin1String( "/dev/shm" ))
                        && (!disk->mountPoint().startsWith(QLatin1String( "/sys/" )) )
                        && (!disk->mountPoint().startsWith(QLatin1String( "/proc/" )) ) )
                {
                    replaceDeviceEntry(disk);
                }
                else
                {
                    delete disk;
                }

            } //if not empty
        } //while
        f.close();
    } //if f.open

    loadSettings(); //to get the mountCommands

    return 1;
}


/***************************************************************************
  * reads the df-commands results
**/
int DiskList::readDF()
{
    qCDebug(KDF);

    if (readingDFStdErrOut || (dfProc->state() != QProcess::NotRunning))
        return -1;

    dfProc->clearProgram();

    QStringList dfenv;
    dfenv << QLatin1String( "LANG=en_US" );
    dfenv << QLatin1String( "LC_ALL=en_US" );
    dfenv << QLatin1String( "LC_MESSAGES=en_US" );
    dfenv << QLatin1String( "LC_TYPE=en_US" );
    dfenv << QLatin1String( "LANGUAGE=en_US" );
    dfenv << QLatin1String( "LC_ALL=POSIX" );
    dfProc->setEnvironment(dfenv);
    dfProc->setProgram(DF_Command,QString(DF_Args).split(QLatin1Char( ' ' )));
    dfProc->start();

    if (!dfProc->waitForStarted(-1))
        qFatal("%s", qPrintable(i18n("could not execute [%1]", QLatin1String(DF_Command))));

    return 1;
}


/***************************************************************************
  * is called, when the df-command has finished
**/
void DiskList::dfDone()
{
    qCDebug(KDF);

    if (updatesDisabled)
        return; //Don't touch the data for now..

    readingDFStdErrOut=true;

    DisksConstIterator itr = disksConstIteratorBegin();
    DisksConstIterator end = disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;
        disk->setMounted(false);  // set all devs unmounted
    }

    QString dfStringErrOut = QString::fromLatin1(dfProc->readAllStandardOutput());
    QTextStream t (&dfStringErrOut, QIODevice::ReadOnly);

    qCDebug(KDF) << t.status();

    QString s;
    while ( !t.atEnd() )
    {
        s = t.readLine();
        if ( s.left(10) == QLatin1String( "Filesystem" ) )
            break;
    }
    if ( t.atEnd() )
        qFatal("Error running df command... got [%s]",qPrintable(s));

    while ( !t.atEnd() )
    {
        QString u,v;
        DiskEntry *disk;
        s=t.readLine();
        s=s.simplified();
        if ( !s.isEmpty() )
        {
            disk = new DiskEntry(); //Q_CHECK_PTR(disk);

            if (!s.contains(Blank))      // devicename was too long, rest in next line
                if ( !t.atEnd() )
                {       // just appends the next line
                    v=t.readLine();
                    s=s.append(v );
                    s=s.simplified();
                }//if silly linefeed


            disk->setDeviceName(s.left(s.indexOf(Blank)) );
            s=s.remove(0,s.indexOf(Blank)+1 );

            if (No_FS_Type)
            {
                disk->setFsType(QLatin1String( "?" ));
            }
            else
            {
                disk->setFsType(s.left(s.indexOf(Blank)) );
                s=s.remove(0,s.indexOf(Blank)+1 );
            };

            u=s.left(s.indexOf(Blank));
            disk->setKBSize(u.toULongLong() );
            s=s.remove(0,s.indexOf(Blank)+1 );

            u=s.left(s.indexOf(Blank));
            disk->setKBUsed(u.toULongLong() );
            s=s.remove(0,s.indexOf(Blank)+1 );

            u=s.left(s.indexOf(Blank));
            disk->setKBAvail(u.toULongLong() );
            s=s.remove(0,s.indexOf(Blank)+1 );


            s=s.remove(0,s.indexOf(Blank)+1 );  // delete the capacity 94%
            disk->setMountPoint(s);

            if ( (disk->kBSize() > 0)
                    && (disk->deviceName() != QLatin1String( "none" ))
                    && (disk->fsType() != QLatin1String( "swap" ))
                    && (disk->fsType() != QLatin1String( "sysfs" ))
                    && (disk->fsType() != QLatin1String( "rootfs" ))
                    && (disk->fsType() != QLatin1String( "tmpfs" ))
                    && (disk->fsType() != QLatin1String( "debugfs" ))
                    && (disk->fsType() != QLatin1String( "devtmpfs" ))
                    && (disk->mountPoint() != QLatin1String( "/dev/swap" ))
                    && (disk->mountPoint() != QLatin1String( "/dev/pts" ))
                    && (disk->mountPoint() != QLatin1String( "/dev/shm" ))
                    && (!disk->mountPoint().startsWith(QLatin1String( "/sys/" )) )
                    && (!disk->mountPoint().startsWith(QLatin1String( "/proc/" )) ) )
            {
                disk->setMounted(true);    // it is now mounted (df lists only mounted)
                replaceDeviceEntry(disk);
            }
            else
            {
                delete disk;
            }

        }//if not header
    }//while further lines available

    readingDFStdErrOut=false;
    loadSettings(); //to get the mountCommands
    emit readDFDone();
}

int DiskList::find( DiskEntry* item )
{

    int pos = -1;
    int i = 0;

    DisksConstIterator itr = disksConstIteratorBegin();
    DisksConstIterator end = disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;
        if ( *item==*disk )
        {
            pos = i;
            break;
        }
        i++;
    }

    return pos;
}

void DiskList::deleteAllMountedAt(const QString &mountpoint)
{
    qCDebug(KDF);

    DisksIterator itr = disksIteratorBegin();
    DisksIterator end = disksIteratorEnd();
    while( itr != end)
    {
        DisksIterator prev = itr;
        ++itr;

        DiskEntry * disk = *prev;
        if (disk->mountPoint() == mountpoint )
        {
            disks->removeOne( disk );
            delete disk;
        }
    }
}

/***************************************************************************
  * updates or creates a new DiskEntry in the KDFList and TabListBox
**/
void DiskList::replaceDeviceEntry(DiskEntry * disk)
{

    //
    // The 'disks' may already already contain the 'disk'. If it do
    // we will replace some data. Otherwise 'disk' will be added to the list
    //

    int pos = -1;
    uint i = 0;

    DisksConstIterator itr = disksConstIteratorBegin();
    DisksConstIterator end = disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * item = *itr;
        if( disk->realCompare(*item) )
        {
            pos = i;
            break;
        }
        i++;
    }

    if ((pos == -1) && (disk->mounted()) )
        // no matching entry found for mounted disk
        if ((disk->fsType() == QLatin1String( "?" )) || (disk->fsType() == QLatin1String( "cachefs" )))
        {
            //search for fitting cachefs-entry in static /etc/vfstab-data
            DiskEntry* olddisk;

            DisksConstIterator itr = disksConstIteratorBegin();
            DisksConstIterator end = disksConstIteratorEnd();
            for (; itr != end; ++itr)
            {
                int p;
                // cachefs deviceNames have no / behind the host-column
                // eg. /cache/cache/.cfs_mnt_points/srv:_home_jesus
                //                                      ^    ^
                olddisk = *itr;

                QString odiskName = olddisk->deviceName();
                int ci=odiskName.indexOf(QLatin1Char( ':' )); // goto host-column
                while ((ci =odiskName.indexOf(QLatin1Char( '/' ),ci)) > 0)
                {
                    odiskName.replace(ci,1,QLatin1String( "_" ));
                }//while
                // check if there is something that is exactly the tail
                // eg. [srv:/tmp3] is exact tail of [/cache/.cfs_mnt_points/srv:_tmp3]
                if ( ( (p=disk->deviceName().lastIndexOf(odiskName
                          ,disk->deviceName().length()) )
                        != -1)
                        && (p + odiskName.length()
                            == disk->deviceName().length()) )
                {
                    pos = disks->indexOf(disk); //store the actual position
                    disk->setDeviceName(olddisk->deviceName());
                }
                //else  olddisk=disks->next();
            }// while
        }// if fsType == "?" or "cachefs"


#ifdef No_FS_Type
    if (pos != -1)
    {
        DiskEntry * olddisk = disks->at(pos);
        if (olddisk)
            disk->setFsType(olddisk->fsType());
    }
#endif

    if (pos != -1)
    {  // replace
        DiskEntry * olddisk = disks->at(pos);
        if ( (olddisk->mountOptions().contains(QLatin1String( "user" ))) &&
                ( disk->mountOptions().contains(QLatin1String( "user" ))) )
        {
            // add "user" option to new diskEntry
            QString s=disk->mountOptions();
            if (s.length()>0)
                s.append(QLatin1String( "," ));
            s.append(QLatin1String( "user" ));
            disk->setMountOptions(s);
        }
        disk->setMountCommand(olddisk->mountCommand());
        disk->setUmountCommand(olddisk->umountCommand());

        // Same device name, but maybe one is a symlink and the other is its target
        // Keep the shorter one then, /dev/hda1 looks better than /dev/ide/host0/bus0/target0/lun0/part1
        // but redefine "shorter" to be the number of slashes in the path as a count on characters
        // breaks legitimate symlinks created by udev
        if ( disk->deviceName().count( QLatin1Char( '/' ) ) > olddisk->deviceName().count( QLatin1Char( '/' ) ) )
            disk->setDeviceName(olddisk->deviceName());

        //FStab after an older DF ... needed for critFull
        //so the DF-KBUsed survive a FStab lookup...
        //but also an unmounted disk may then have a kbused set...
        if ( (olddisk->mounted()) && (!disk->mounted()) )
        {
            disk->setKBSize(olddisk->kBSize());
            disk->setKBUsed(olddisk->kBUsed());
            disk->setKBAvail(olddisk->kBAvail());
        }
        if ( (olddisk->percentFull() != -1) &&
                (olddisk->percentFull() <  Full_Percent) &&
                (disk->percentFull() >= Full_Percent) )
        {
            qCDebug(KDF) << "Device " << disk->deviceName()
            << " is critFull! " << olddisk->percentFull()
            << "--" << disk->percentFull() << endl;
            emit criticallyFull(disk);
        }

        //Take the diskentry from the list and delete it properly
        DiskEntry * tmp = disks->takeAt(pos);
        delete tmp;

        disks->insert(pos,disk);
    }
    else
    {
        disks->append(disk);
    }//if

}

#include "disklist.moc"

