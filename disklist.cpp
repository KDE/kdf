/*
 * disklist.cpp
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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

#include <math.h>
#include <stdlib.h>
#include <kdebug.h>

#include <QtCore/QTextStream>
#include <kglobal.h>
#include <kconfiggroup.h>
#include <kdefakes.h>
#include <kprocess.h>

#define BLANK ' '
#define DELIMITER '#'
#define FULL_PERCENT 95.0

/***************************************************************************
  * constructor
**/
DiskList::DiskList(QObject *parent)
    : QObject(parent), dfProc(new KProcess(this))
{
   kDebug() ;

   updatesDisabled = false;

   if (NO_FS_TYPE) {
      kDebug() << "df gives no FS_TYPE" ;
   }

   disks = new Disks;
   disks->setAutoDelete(true);

   // BackgroundProcesses ****************************************
   dfProc->setOutputChannelMode(KProcess::MergedChannels);
   connect(dfProc,SIGNAL(finished(int, QProcess::ExitStatus) ),
      this, SLOT(dfDone() ) );

   readingDFStdErrOut=false;
   config = KGlobal::config();
   loadSettings();
}


/***************************************************************************
  * destructor
**/
DiskList::~DiskList()
{
   kDebug() ;
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
  kDebug() ;

  KConfigGroup group(config, "DiskList");
  QString key;
  DiskEntry *disk;
  for (disk=disks->first();disk!=0;disk=disks->next()) {
   key = QLatin1String("Mount") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
   group.writePathEntry(key,disk->mountCommand());

   key = QLatin1String("Umount") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
   group.writePathEntry(key,disk->umountCommand());

   key = QLatin1String("Icon") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
   group.writePathEntry(key,disk->realIconName());
 }
 group.sync();
}


/***************************************************************************
  * reads the KConfig for special mount/umount scripts
**/
void DiskList::loadSettings()
{
  kDebug() ;

  const KConfigGroup group(config, "DiskList");
  QString key;
  DiskEntry *disk;
  for (disk=disks->first();disk!=0;disk=disks->next()) {
    key = QLatin1String("Mount") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
    disk->setMountCommand(group.readPathEntry(key, QString()));

    key = QLatin1String("Umount") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
    disk->setUmountCommand(group.readPathEntry(key, QString()));

    key = QLatin1String("Icon") + SEPARATOR + disk->deviceName() + SEPARATOR + disk->mountPoint();
    QString icon=group.readPathEntry(key, QString());
    if (!icon.isEmpty()) disk->setIconName(icon);
 }
}


static QString expandEscapes(const QString& s) {
QString rc;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '\\') {
            i++;
			QChar str=s.at(i);
			if( str == '\\')
					rc += '\\';
			else if( str == '0')
			{
					rc += static_cast<char>(s.mid(i,3).toULongLong(0, 8));
					i += 2;
			}
			else
			{
               // give up and not process anything else because I'm too lazy
               // to implement other escapes
               rc += '\\';
               rc += s[i];
			}
        } else {
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
  kDebug() ;

  if (readingDFStdErrOut || (dfProc->state() != QProcess::NotRunning)) return -1;

QFile f(FSTAB);
  if ( f.open(QIODevice::ReadOnly) ) {
    QTextStream t (&f);
    QString s;
    DiskEntry *disk;

    //disks->clear(); // ############

    while (! t.atEnd()) {
      s=t.readLine();
      s=s.simplified();
      if ( (!s.isEmpty() ) && (s.indexOf(DELIMITER)!=0) ) {
               // not empty or commented out by '#'
	//	kDebug() << "GOT: [" << s << "]" ;
	disk = new DiskEntry();// Q_CHECK_PTR(disk);
        disk->setMounted(false);
        disk->setDeviceName(expandEscapes(s.left(s.indexOf(BLANK))));
            s=s.remove(0,s.indexOf(BLANK)+1 );
	    //  kDebug() << "    deviceName:    [" << disk->deviceName() << "]" ;
#ifdef _OS_SOLARIS_
            //device to fsck
            s=s.remove(0,s.indexOf(BLANK)+1 );
#endif
         disk->setMountPoint(expandEscapes(s.left(s.indexOf(BLANK))));
            s=s.remove(0,s.indexOf(BLANK)+1 );
	    //kDebug() << "    MountPoint:    [" << disk->mountPoint() << "]" ;
	    //kDebug() << "    Icon:          [" << disk->iconName() << "]" ;
         disk->setFsType(s.left(s.indexOf(BLANK)) );
            s=s.remove(0,s.indexOf(BLANK)+1 );
	    //kDebug() << "    FS-Type:       [" << disk->fsType() << "]" ;
         disk->setMountOptions(s.left(s.indexOf(BLANK)) );
            s=s.remove(0,s.indexOf(BLANK)+1 );
	    //kDebug() << "    Mount-Options: [" << disk->mountOptions() << "]" ;
         if ( (disk->deviceName() != "none")
	      && (disk->fsType() != "swap")
	      && (disk->fsType() != "sysfs")
	      && (disk->mountPoint() != "/dev/swap")
	      && (disk->mountPoint() != "/dev/pts")
	      && (disk->mountPoint() != "/dev/shm")
	      && (!disk->mountPoint().contains("/proc") ) )
	   replaceDeviceEntry(disk);
         else
           delete disk;

      } //if not empty
    } //while
    f.close();
  } //if f.open

  loadSettings(); //to get the mountCommands

  //  kDebug() << "DiskList::readFSTAB DONE" ;
  return 1;
}


/***************************************************************************
  * reads the df-commands results
**/
int DiskList::readDF()
{
  kDebug() ;

  if (readingDFStdErrOut || (dfProc->state() != QProcess::NotRunning)) return -1;
  
  dfProc->clearProgram();

  QStringList dfenv;
  dfenv << "LANG=en_US";
  dfenv << "LC_ALL=en_US";
  dfenv << "LC_MESSAGES=en_US";
  dfenv << "LC_TYPE=en_US";
  dfenv << "LANGUAGE=en_US";
  dfenv << "LC_ALL=POSIX";
  dfProc->setEnvironment(dfenv);
  (*dfProc) << DF_COMMAND << DF_ARGS;
  dfProc->start();
  if (!dfProc->waitForStarted(-1))
    qFatal("%s", qPrintable(i18n("could not execute [%1]", QLatin1String(DF_COMMAND))));
  return 1;
}


/***************************************************************************
  * is called, when the df-command has finished
**/
void DiskList::dfDone()
{
  kDebug() ;

  if (updatesDisabled)
      return; //Don't touch the data for now..

  readingDFStdErrOut=true;
  for ( DiskEntry *disk=disks->first(); disk != 0; disk=disks->next() )
    disk->setMounted(false);  // set all devs unmounted

  QString dfStringErrOut = QString::fromLatin1(dfProc->readAllStandardOutput());
  QTextStream t (&dfStringErrOut, QIODevice::ReadOnly);
  QString s=t.readLine();
  if ( ( s.isEmpty() ) || ( s.left(10) != "Filesystem" ) )
    qFatal("Error running df command... got [%s]",qPrintable(s));
  while ( !t.atEnd() ) {
    QString u,v;
    DiskEntry *disk;
    s=t.readLine();
    s=s.simplified();
    if ( !s.isEmpty() ) {
      disk = new DiskEntry(); Q_CHECK_PTR(disk);

      if (!s.contains(BLANK))      // devicename was too long, rest in next line
	if ( !t.atEnd() ) {       // just appends the next line
            v=t.readLine();
            s=s.append(v );
            s=s.simplified();
	    //kDebug() << "SPECIAL GOT: [" << s << "]" ;
	 }//if silly linefeed

      //kDebug() << "EFFECTIVELY GOT " << s.length() << " chars: [" << s << "]" ;

      disk->setDeviceName(s.left(s.indexOf(BLANK)) );
      s=s.remove(0,s.indexOf(BLANK)+1 );
      //kDebug() << "    DeviceName:    [" << disk->deviceName() << "]" ;

      if (NO_FS_TYPE) {
	//kDebug() << "THERE IS NO FS_TYPE_FIELD!" ;
         disk->setFsType("?");
      } else {
         disk->setFsType(s.left(s.indexOf(BLANK)) );
         s=s.remove(0,s.indexOf(BLANK)+1 );
      };
      //kDebug() << "    FS-Type:       [" << disk->fsType() << "]" ;
      //kDebug() << "    Icon:          [" << disk->iconName() << "]" ;

      u=s.left(s.indexOf(BLANK));
      disk->setKBSize(u.toULongLong() );
      s=s.remove(0,s.indexOf(BLANK)+1 );
      //kDebug() << "    Size:       [" << disk->kBSize() << "]" ;

      u=s.left(s.indexOf(BLANK));
      disk->setKBUsed(u.toULongLong() );
      s=s.remove(0,s.indexOf(BLANK)+1 );
      //kDebug() << "    Used:       [" << disk->kBUsed() << "]" ;

      u=s.left(s.indexOf(BLANK));
      disk->setKBAvail(u.toULongLong() );
      s=s.remove(0,s.indexOf(BLANK)+1 );
      //kDebug() << "    Avail:       [" << disk->kBAvail() << "]" ;


      s=s.remove(0,s.indexOf(BLANK)+1 );  // delete the capacity 94%
      disk->setMountPoint(s);
      //kDebug() << "    MountPoint:       [" << disk->mountPoint() << "]" ;

      if ( (disk->kBSize() > 0)
	   && (disk->deviceName() != "none")
	   && (disk->fsType() != "swap")
	   && (disk->fsType() != "sysfs")
	   && (disk->mountPoint() != "/dev/swap")
	   && (disk->mountPoint() != "/dev/pts")
	   && (disk->mountPoint() != "/dev/shm")
	   && (!disk->mountPoint().contains("/proc") ) ) {
        disk->setMounted(true);    // its now mounted (df lists only mounted)
	replaceDeviceEntry(disk);
      } else
	delete disk;

    }//if not header
  }//while further lines available

  readingDFStdErrOut=false;
  loadSettings(); //to get the mountCommands
  emit readDFDone();
}


void DiskList::deleteAllMountedAt(const QString &mountpoint)
{
  kDebug() ;


    for ( DiskEntry *item  = disks->first(); item;  )
    {
        if (item->mountPoint() == mountpoint ) {
            kDebug() << "delete " << item->deviceName() ;
            disks->remove(item);
            item = disks->current();
        } else
            item = disks->next();
    }
}

/***************************************************************************
  * updates or creates a new DiskEntry in the KDFList and TabListBox
**/
void DiskList::replaceDeviceEntry(DiskEntry *disk)
{
  //kDebug() << disk->deviceRealName() << " " << disk->realMountPoint() ;

  //
  // The 'disks' may already already contain the 'disk'. If it do
  // we will replace some data. Otherwise 'disk' will be added to the list
  //

  //
  // 1999-27-11 Espen Sand:
  // I can't get find() to work. The Disks::compareItems(..) is
  // never called.
  //
  //int pos=disks->find(disk);

  QString deviceRealName = disk->deviceRealName();
  QString realMountPoint = disk->realMountPoint();

  int pos = -1;
  for( u_int i=0; i<disks->count(); i++ )
  {
    DiskEntry *item = disks->at(i);
    int res = deviceRealName.compare( item->deviceRealName() );
    if( res == 0 )
    {
      res = realMountPoint.compare( item->realMountPoint() );
    }
    if( res == 0 )
    {
      pos = i;
      break;
    }
  }

  if ((pos == -1) && (disk->mounted()) )
    // no matching entry found for mounted disk
    if ((disk->fsType() == "?") || (disk->fsType() == "cachefs")) {
      //search for fitting cachefs-entry in static /etc/vfstab-data
      DiskEntry* olddisk = disks->first();
      while (olddisk != 0) {
        int p;
        // cachefs deviceNames have no / behind the host-column
	// eg. /cache/cache/.cfs_mnt_points/srv:_home_jesus
	//                                      ^    ^
        QString odiskName = olddisk->deviceName();
        int ci=odiskName.indexOf(':'); // goto host-column
        while ((ci =odiskName.indexOf('/',ci)) > 0) {
           odiskName.replace(ci,1,"_");
        }//while
        // check if there is something that is exactly the tail
	// eg. [srv:/tmp3] is exact tail of [/cache/.cfs_mnt_points/srv:_tmp3]
        if ( ( (p=disk->deviceName().lastIndexOf(odiskName
	            ,disk->deviceName().length()) )
                != -1)
	      && (p + odiskName.length()
	          == disk->deviceName().length()) )
        {
             pos = disks->at(); //store the actual position
             disk->setDeviceName(olddisk->deviceName());
             olddisk=0;
	} else
          olddisk=disks->next();
      }// while
    }// if fsType == "?" or "cachefs"


#ifdef NO_FS_TYPE
  if (pos != -1) {
     DiskEntry * olddisk = disks->at(pos);
     if (olddisk)
        disk->setFsType(olddisk->fsType());
  }
#endif

  if (pos != -1) {  // replace
      DiskEntry * olddisk = disks->at(pos);
      if ( (olddisk->mountOptions().contains("user")) &&
           ( disk->mountOptions().contains("user")) ) {
          // add "user" option to new diskEntry
          QString s=disk->mountOptions();
          if (s.length()>0) s.append(",");
          s.append("user");
          disk->setMountOptions(s);
       }
      disk->setMountCommand(olddisk->mountCommand());
      disk->setUmountCommand(olddisk->umountCommand());

      // Same device name, but maybe one is a symlink and the other is its target
      // Keep the shorter one then, /dev/hda1 looks better than /dev/ide/host0/bus0/target0/lun0/part1
      // but redefine "shorter" to be the number of slashes in the path as a count on characters
      // breaks legitimate symlinks created by udev
      if ( disk->deviceName().count( '/' ) > olddisk->deviceName().count( '/' ) )
          disk->setDeviceName(olddisk->deviceName());

      //FStab after an older DF ... needed for critFull
      //so the DF-KBUsed survive a FStab lookup...
      //but also an unmounted disk may then have a kbused set...
      if ( (olddisk->mounted()) && (!disk->mounted()) ) {
         disk->setKBSize(olddisk->kBSize());
         disk->setKBUsed(olddisk->kBUsed());
         disk->setKBAvail(olddisk->kBAvail());
      }
          if ( (olddisk->percentFull() != -1) &&
               (olddisk->percentFull() <  FULL_PERCENT) &&
                  (disk->percentFull() >= FULL_PERCENT) ) {
	    kDebug() << "Device " << disk->deviceName()
		      << " is critFull! " << olddisk->percentFull()
		      << "--" << disk->percentFull() << endl;
            emit criticallyFull(disk);
	  }
      disks->remove(pos); // really deletes old one
      disks->insert(pos,disk);
  } else {
    disks->append(disk);
  }//if

}

#include "disklist.moc"






