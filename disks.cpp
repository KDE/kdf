/*
 * disks.cpp
 *
 * Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qregexp.h>

#include <kapp.h>
#include "disks.h"
#include "disks.moc"

/****************************************************/
/********************* DiskEntry ********************/
/****************************************************/

/**
  * Constructor
**/
void DiskEntry::init()
{
  device="";
  type="";
  mountedOn="";
  options="";
  size=0;
  used=0;
  avail=0;
  isMounted=FALSE;
  mntcmd="";
  umntcmd="";
  iconSetByUser=FALSE;
  icoName="";


 // BackgroundProcesses ****************************************

 sysProc = new KShellProcess(); CHECK_PTR(sysProc);
 connect( sysProc, SIGNAL(receivedStdout(KProcess *, char *, int) ),
        this, SLOT (receivedSysStdErrOut(KProcess *, char *, int)) );
 connect( sysProc, SIGNAL(receivedStderr(KProcess *, char *, int) ),
        this, SLOT (receivedSysStdErrOut(KProcess *, char *, int)) );
 readingSysStdErrOut=FALSE;


}

DiskEntry::DiskEntry(QObject *parent, const char *name)
 : QObject (parent, name)
{
  init();
}

DiskEntry::DiskEntry(QString deviceName, QObject *parent, const char *name)
 : QObject (parent, name)
{
  init();
  setDeviceName(deviceName);
}
DiskEntry::~DiskEntry()
{
  //debug("DESTRUCT: DiskList"); 
  disconnect(this);
  delete sysProc;
};

int DiskEntry::toggleMount()
{ 
  if (!mounted())
      return mount();
  else
      return umount();
};

int DiskEntry::mount()
{ 
  //debug("mounting");
  QString cmdS=mntcmd;
  if (cmdS.isEmpty()) // generate default mount cmd
    if (getuid()!=0 ) // user mountable
      cmdS="mount %d";
	else  // root mounts with all params/options
      cmdS="mount -t%t %d %m -o %o";

  cmdS.replace(QRegExp("%d"),this->deviceName());
  cmdS.replace(QRegExp("%m"),this->mountPoint());
  cmdS.replace(QRegExp("%t"),this->fsType());
  cmdS.replace(QRegExp("%o"),this->mountOptions());

  debug("mount-cmd: [%s]",(const char *)cmdS);
  int e=sysCall(cmdS);
  if (!e) setMounted(TRUE);
  return e;
};

int DiskEntry::umount()
{
  //debug("umounting");
  QString cmdS=umntcmd;
  if (cmdS.isEmpty()) // generate default umount cmd
      cmdS="umount %d";

  cmdS.replace(QRegExp("%d"),this->deviceName());
  cmdS.replace(QRegExp("%m"),this->mountPoint());

  debug("umount-cmd: [%s]",(const char *)cmdS);
  int e=sysCall(cmdS);
  if (!e) setMounted(FALSE);
 return e;
};

int DiskEntry::remount()
{
  QString cmdS;

  if (mntcmd.isEmpty() && umntcmd.isEmpty() // default mount/umount commands
      && (getuid()==0)) // you are root
    {
    QString oldOpt=options;
    if (options.isEmpty())
       options="remount";
    else
       options+=",remount";
    int e=this->mount();
    options=oldOpt;
    return e;
   } else {
    if (int e=this->umount()) 
      return this->mount();
   else return e;
  }
};
void DiskEntry::setMountCommand(QString mnt) 
{
  mntcmd=mnt;
};

void DiskEntry::setUmountCommand(QString umnt) 
{
  umntcmd=umnt;
};

void DiskEntry::setIconName(QString iconName) 
{
  iconSetByUser=TRUE;
  icoName=iconName;
  emit iconNameChanged();
};

QString DiskEntry::iconName() 
{
  QString iconName=icoName;
  if (iconSetByUser) {
   mounted() ? iconName+="_mount.xpm" : iconName+="_unmount.xpm";
   return iconName;
  } else
   return guessIconName();
};

QString DiskEntry::guessIconName()
{
  QString iconName;
    // try to be intelligent
    if (-1!=mountPoint().find("cdrom",0,FALSE)) iconName="cdrom";
    else if (-1!=deviceName().find("cdrom",0,FALSE)) iconName="cdrom";
    else if (-1!=deviceName().find("fd",0,FALSE)) {
            if (-1!=deviceName().find("360",0,FALSE)) iconName="5floppy";
            if (-1!=deviceName().find("1200",0,FALSE)) iconName="5floppy";
            else iconName="3floppy";
	 }
    else if (-1!=mountPoint().find("floppy",0,FALSE)) iconName="3floppy";
    else if (-1!=mountPoint().find("zip",0,FALSE)) iconName="zip";
    else if (-1!=fsType().find("nfs",0,FALSE)) iconName="nfs";
    else iconName="harddrive";
    mounted() ? iconName+="_mount.xpm" : iconName+="_unmount.xpm";
//    if ( -1==mountOptions().find("user",0,FALSE) )
//      iconName.prepend("root_"); // special root icon, normal user can´t mount

    //debug("file is [%s]",iconName.data());
    if ( (!QFile::exists(QString(kapp->kde_icondir()
                              +QString("/mini/")+iconName).data())) 
        && (!QFile::exists(QString(kapp->kde_icondir()
                              +QString("/")+iconName).data())) )
    {
      warning("file [%s] doesn't exist (not even mini)!",iconName.data());
      iconName="unknown.xpm";
    }
    
    //debug("device %s is %s",deviceName().data(),iconName.data());
    
    //emit iconNameChanged();
  return iconName;
};


/***************************************************************************
  * starts a command on the underlying system via /bin/sh
**/
int DiskEntry::sysCall(QString command)
{
  //debug("DiskEntry::sysCall");
  if (readingSysStdErrOut || sysProc->isRunning() )  return -1;

  sysStringErrOut="called: "+command+"\n\n"; // put the called command on ErrOut
  sysProc->clearArguments();
  (*sysProc) << (const char *)command;
    if (!sysProc->start( KProcess::Block, KProcess::AllOutput ))
     fatal(klocale->translate("could not execute [%s]"),(const char *)command);
  if (sysProc->exitStatus()!=0) emit sysCallError(this, sysProc->exitStatus());
  return !sysProc->exitStatus();  
};


/***************************************************************************
  * is called, when the Sys-command writes on StdOut or StdErr
**/
void DiskEntry::receivedSysStdErrOut(KProcess *, char *data, int len)
{
  //debug("DiskEntry::receivedSysStdErrOut");
  QString tmp = QString(data,len+1);  // adds a zero-byte 
  sysStringErrOut.append(tmp);
};

QString DiskEntry::prettyPrint(int kBValue) const
{
  QString weight;
  float val=(float)kBValue; // size in KiloByte

  //always go up to MegaByte
  val=val/1024;
  weight="MB";

  if (val>999.0) {  //GigaByte
    val=val/1024;
    weight="GB";
  }//if

  QString ret;
  if (val>100.0)  // e.g. 504MB
    ret.sprintf("%3.0f%s",val,(const char *)weight);
  else
    if (val>10.0) // e.g. 54.7MB
      ret.sprintf("%3.1f%s",val,(const char *)weight);
    else // e.g. 1.44KB
      ret.sprintf("%3.2f%s",val,(const char *)weight);
  return ret;
}

float DiskEntry::percentFull() const
{
   if (size != 0) {
      return 100 - ( ((float)avail / (float)size) * 100 );
   } else {
      return -1;
   }
}

void DiskEntry::setDeviceName(QString deviceName)
{
 device=deviceName; 
 emit deviceNameChanged();
};

void DiskEntry::setMountPoint(QString mountPoint)
{
  mountedOn=mountPoint;
 emit mountPointChanged();
};

void DiskEntry::setMountOptions(QString mountOptions)
{
 options=mountOptions;
 emit mountOptionsChanged();
};

void DiskEntry::setFsType(QString fsType)
{
  type=fsType;
  emit fsTypeChanged();
};

void DiskEntry::setMounted(bool nowMounted) 
{ 
  isMounted=nowMounted;
  emit mountedChanged();
};
  
void DiskEntry::setKBSize(int kb_size)
{
  //debug("DiskEntry::setKBSize(%d)",kb_size);
  size=kb_size;
  emit kBSizeChanged();
};

void DiskEntry::setKBUsed(int kb_used)
{
  //debug("DiskEntry::setKBUsed(%d)",kb_used);
  used=kb_used;
  if ( size < (used+avail) ) {  //adjust kBAvail
     warning("device %s: kBAvail(%d)+*kBUsed(%d) exceeds kBSize(%d)"
           ,(const char *)device,avail,used,size);
     setKBAvail(size-used); 
  }
  emit kBUsedChanged();
};

void DiskEntry::setKBAvail(int kb_avail)
{
  //debug("DiskEntry::setKBAvail(%d)",kb_avail);
  avail=kb_avail;
  if ( size < (used+avail) ) {  //adjust kBUsed
     warning("device %s: *kBAvail(%d)+kBUsed(%d) exceeds kBSize(%d)"
           ,(const char *)device,avail,used,size);
     setKBUsed(size-avail); 
  }
  emit kBAvailChanged();
};


