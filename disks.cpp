/*
 * disks.cpp
 *
 * Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
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

#include "disks.h"

#include <unistd.h>
#include <sys/types.h>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <kglobal.h>
#include <kdebug.h>
#include <kprocess.h>
#include <klocale.h>
//#include <solid/device.h>

/****************************************************/
/********************* DiskEntry ********************/
/****************************************************/

/**
  * Constructor
**/
void DiskEntry::init(const char *name)
{
    setObjectName( QLatin1String( name ) );
    device.clear();
    type.clear();
    mountedOn.clear();
    options.clear();
    size=0;
    used=0;
    avail=0;
    isMounted=false;
    mntcmd.clear();
    umntcmd.clear();
    iconSetByUser=false;
    icoName.clear();


    // BackgroundProcesses ****************************************

    sysProc = new KProcess();
    Q_CHECK_PTR(sysProc);
    sysProc->setOutputChannelMode( KProcess::MergedChannels );
    connect( sysProc, SIGNAL(readyReadStandardError()),
             this, SLOT (receivedSysStdErrOut()) );
    connect( sysProc, SIGNAL(readyReadStandardOutput()),
             this, SLOT (receivedSysStdErrOut()) );
    readingSysStdErrOut=false;


}

DiskEntry::DiskEntry(QObject *parent, const char *name)
        : QObject (parent)
{
    init(name);
}

DiskEntry::DiskEntry(const QString & deviceName, QObject *parent, const char *name)
        : QObject (parent)
{
    init(name);

    setDeviceName(deviceName);
}
DiskEntry::~DiskEntry()
{
    disconnect(this);
    if ( sysProc->state() == QProcess::Running )
    {
        sysProc->terminate();
        sysProc->waitForFinished(-1);
    }
    delete sysProc;
}

int DiskEntry::toggleMount()
{
    if (!mounted())
        return mount();
    else
        return umount();
}

int DiskEntry::mount()
{
    QString cmdS=mntcmd;
    if ( cmdS.isEmpty() )
    { // generate default mount cmd
        if ( getuid()!=0 ) // user mountable
        {
            cmdS = QLatin1String( "mount %d" );
        }
        else  // root mounts with all params/options
        {
            // FreeBSD's mount(8) is picky: -o _must_ go before
            // the device and mountpoint.
            cmdS = QLatin1String( "mount -t%t -o%o %d %m" );
        }
    }

    cmdS.replace( QLatin1String( "%d" ), deviceName() );
    cmdS.replace( QLatin1String( "%m" ), mountPoint() );
    cmdS.replace( QLatin1String( "%t" ), fsType() );
    cmdS.replace( QLatin1String( "%o" ), mountOptions() );

    kDebug() << "mount-cmd: [" << cmdS << "]" ;
    int e = sysCall( cmdS );
    if (!e)
        setMounted( true );
    kDebug() << "mount-cmd: e=" << e ;
    return e;
}

int DiskEntry::umount()
{
    kDebug() << "umounting" ;
    QString cmdS = umntcmd;
    if ( cmdS.isEmpty() ) // generate default umount cmd
        cmdS = QLatin1String( "umount %d" );

    cmdS.replace( QLatin1String( "%d" ), deviceName() );
    cmdS.replace( QLatin1String( "%m" ), mountPoint() );

    kDebug() << "umount-cmd: [" << cmdS << "]" ;
    int e = sysCall( cmdS );
    if ( !e )
        setMounted( false );
    kDebug() << "umount-cmd: e=" << e ;

    return e;
}

int DiskEntry::remount()
{
    if ( mntcmd.isEmpty() && umntcmd.isEmpty() // default mount/umount commands
            && ( getuid()==0) ) // you are root
    {
        QString oldOpt = options;
        if (options.isEmpty())
            options = QLatin1String( "remount" );
        else
            options += QLatin1String( ",remount" );

        int e = mount();
        options = oldOpt;
        return e;
    } else
    {
        if ( int e=umount() )
            return mount();
        else
            return e;
    }
}

void DiskEntry::setMountCommand(const QString & mnt)
{
    mntcmd=mnt;
}

void DiskEntry::setUmountCommand(const QString & umnt)
{
    umntcmd=umnt;
}

void DiskEntry::setIconName(const QString & iconName)
{
    iconSetByUser=true;
    icoName=iconName;
    if ( icoName.right(6) == QLatin1String( "_mount" ) )
        icoName.truncate(icoName.length()-6);
    else if ( icoName.right(8) == QLatin1String( "_unmount" ) )
        icoName.truncate(icoName.length()-8);

    emit iconNameChanged();
}

void DiskEntry::setIconToDefault()
{
    iconSetByUser = false;
    icoName.clear();

}

QString DiskEntry::iconName()
{
    if (iconSetByUser)
        return icoName;
    else
        return guessIconName();
}

//TODO: Need porting to solid
QString DiskEntry::guessIconName()
{
    QString iconName;

    /*
    //List Solid Devices
    foreach (const Solid::Device &device, Solid::Device::listFromType(Solid::DeviceInterface::StorageVolume))
      {
          kDebug() << device.udi().toLatin1().constData() << device.vendor() << device.product() << device.icon();
      }
    Solid::Device * device = new Solid::Device(deviceName());
    kDebug() << "guess" << deviceName() << device->icon();
    delete device;
    */
    // try to be intelligent
    if (mountPoint().contains(QLatin1String( "cdrom" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "media-optical" );
    else if (deviceName().contains(QLatin1String( "cdrom" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "media-optical" );
    else if (mountPoint().contains(QLatin1String( "writer" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "media-optical-recordable" );
    else if (deviceName().contains(QLatin1String( "writer" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "media-optical-recordable" );
    else if (mountPoint().contains(QLatin1String( "mo" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "mo" ); //TODO
    else if (deviceName().contains(QLatin1String( "mo" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "mo" ); //TODO
    else if (deviceName().contains(QLatin1String( "fd" ),Qt::CaseInsensitive))
    {
        if (deviceName().contains(QLatin1String( "360" ),Qt::CaseInsensitive))
            iconName+=QLatin1String( "5floppy" ); //TODO
        if (deviceName().contains(QLatin1String( "1200" ),Qt::CaseInsensitive))
            iconName+=QLatin1String( "5floppy" ); //TODO
        else
            iconName+=QLatin1String( "media-floppy" );
    }
    else if (mountPoint().contains(QLatin1String( "floppy" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "media-floppy" );
    else if (mountPoint().contains(QLatin1String( "zip" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "zip" ); //TODO
    else if (fsType().contains(QLatin1String( "nfs" ),Qt::CaseInsensitive))
        iconName+=QLatin1String( "nfs" ); //TODO
    else
        iconName+=QLatin1String( "drive-harddisk" );
    ///mounted() ? iconName+="_mount" : iconName+="_unmount";
    //    if ( !mountOptions().contains("user",Qt::CaseInsensitive) )
    //      iconName.prepend("root_"); // special root icon, normal user can't mount

    //debug("device %s is %s",deviceName().latin1(),iconName.latin1());

    //emit iconNameChanged();
    return iconName;
}


/***************************************************************************
  * starts a command on the underlying system via /bin/sh
**/
int DiskEntry::sysCall(QString & completeCommand)
{
    if (readingSysStdErrOut || sysProc->state() == QProcess::Running )
        return -1;

    sysStringErrOut=i18n("Called: %1\n\n", completeCommand); // put the called command on ErrOut
    sysProc->clearProgram();

    //Split command and arguments to use the new API, otherwise it doesn't work
    QTextStream tS(&completeCommand);

    QString command;
    tS >> command;

    QString tmp;
    QStringList args;
    while( !tS.atEnd() )
    {
        tS >> tmp;
        args << tmp;
    }

    sysProc->setProgram(command, args);
    sysProc->start();

    if ( !sysProc->waitForStarted(-1) )
        kFatal() << i18n("could not execute %1", command) ;

    sysProc->waitForFinished(-1);

    if (sysProc->exitCode()!=0)
        emit sysCallError(this, sysProc->exitStatus());

    return (sysProc->exitCode());
}


/***************************************************************************
  * is called, when the Sys-command writes on StdOut or StdErr
**/
void DiskEntry::receivedSysStdErrOut()
{
    QString stdOut = QString::fromLocal8Bit( sysProc->readAllStandardOutput() );
    QString stdErr = QString::fromLocal8Bit( sysProc->readAllStandardError() );

    sysStringErrOut.append( stdOut );
    sysStringErrOut.append( stdErr );
}

float DiskEntry::percentFull() const
{
    if (size != 0)
    {
        return 100 - ( ((float)avail / (float)size) * 100 );
    }
    else
    {
        return -1;
    }
}

void DiskEntry::setDeviceName(const QString & deviceName)
{
    device=deviceName;
    emit deviceNameChanged();
}

QString DiskEntry::deviceRealName() const
{
    QFileInfo inf( device );
    QDir dir( inf.absolutePath() );
    QString relPath = inf.fileName();
    if ( inf.isSymLink() )
    {
        QString link = inf.readLink();
        if ( link.startsWith( QLatin1Char( '/' ) ) )
            return link;
        relPath = link;
    }
    return dir.canonicalPath() + QLatin1Char( '/' ) + relPath;
}

void DiskEntry::setMountPoint(const QString & mountPoint)
{
    mountedOn=mountPoint;
    emit mountPointChanged();
}

QString DiskEntry::realMountPoint() const
{
    QDir dir( mountedOn );
    return dir.canonicalPath();
}

void DiskEntry::setMountOptions(const QString & mountOptions)
{
    options=mountOptions;
    emit mountOptionsChanged();
}

void DiskEntry::setFsType(const QString & fsType)
{
    type=fsType;
    emit fsTypeChanged();
}

void DiskEntry::setMounted(bool nowMounted)
{
    isMounted=nowMounted;
    emit mountedChanged();
}

void DiskEntry::setKBSize(qulonglong kb_size)
{
    size=kb_size;
    emit kBSizeChanged();
}

void DiskEntry::setKBUsed(qulonglong kb_used)
{
    used=kb_used;
    if ( size < (used+avail) )
    {  //adjust kBAvail
        kWarning() << "device " << device << ": kBAvail(" << avail << ")+*kBUsed(" << used << ") exceeds kBSize(" << size << ")" ;
        setKBAvail(size-used);
    }
    emit kBUsedChanged();
}

void DiskEntry::setKBAvail(qulonglong kb_avail)
{
    avail=kb_avail;
    if ( size < (used+avail) )
    {  //adjust kBUsed
        kWarning() << "device " << device << ": *kBAvail(" << avail << ")+kBUsed(" << used << ") exceeds kBSize(" << size << ")" ;
        setKBUsed(size-avail);
    }
    emit kBAvailChanged();
}

#include "disks.moc"

