/*
* disks.h
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

#ifndef DISKS_H
#define DISKS_H

#include <QtCore/QObject>

#include <kio/global.h>

class KProcess;

class DiskEntry : public QObject
{
    Q_OBJECT
    
    public:
        explicit DiskEntry(QObject *parent=0, const char *name=0);
        explicit DiskEntry(const QString & deviceName, QObject *parent=0, const char *name=0);
        ~DiskEntry();
        QString lastSysError() {return sysStringErrOut; }
        QString deviceName() const { return device; }
        // The real device (in case deviceName() is a symlink)
        QString deviceRealName() const;
        QString mountPoint() const { return mountedOn; }
        QString mountOptions() const { return options; }
        // The real device (in case deviceName() is a symlink)
        QString realMountPoint() const;
        /**
        * sets the used mountCommand for the actual DiskEntry.
        * @param mntcmd   is a string containing the executable file and
        *                 special codes which will be filled in when used: <BR>
        *                 %m : mountpoint <BR>
        *                 %d : deviceName <BR>
        *                 %t : filesystem type <BR>
        *                 %o : mount options <BR>
        *                 all this information is gained from the objects' data
        *                 if no mountCommand is set it defaults to "mount %d"
        **/
        QString mountCommand() const { return mntcmd; }
        /**
        * sets the used umountCommand for the actual DiskEntry.
        * @param mntcmd   is a string containing the executable file and
        *                 special codes which will be filled in when used: <BR>
        *                 %m : mountpoint <BR>
        *                 %d : deviceName <BR>
        *                 all this information is gained from the objects' data
        *                 if no umountCommand is set it defaults to "umount %d"
        **/
        QString umountCommand() const { return umntcmd; }
        QString fsType() const { return type; }
        bool mounted() const { return isMounted; }
        qulonglong kBSize() const { return size; }
        QString iconName();
        QString realIconName() { return icoName; }
        QString prettyKBSize() const { return KIO::convertSizeFromKiB(size); }
        qulonglong kBUsed() const { return used; }
        QString prettyKBUsed() const { return KIO::convertSizeFromKiB(used); }
        qulonglong kBAvail() const  { return avail; }
        QString prettyKBAvail() const { return KIO::convertSizeFromKiB(avail); }
        float percentFull() const;
        // == comparison
        bool operator==( const DiskEntry & s2 ) const
        {
            bool ret = this->deviceName() == s2.deviceName();
            if( ret )
                ret = this->mountPoint()  == s2.mountPoint();
            
            return( ret );
        }
        // Comparison using *real* device and mountpoint
        bool realCompare( const DiskEntry & s2 ) const
        {
            bool ret = this->deviceRealName() == s2.deviceRealName();
            if( ret )
                ret = this->realMountPoint()  == s2.realMountPoint();
            
            return( ret );
        }
        
    Q_SIGNALS:
        void sysCallError(DiskEntry *disk, int err_no);
        void deviceNameChanged();
        void mountPointChanged();
        void mountOptionsChanged();
        void fsTypeChanged();
        void mountedChanged();
        void kBSizeChanged();
        void kBUsedChanged();
        void kBAvailChanged();
        void iconNameChanged();
        
    public Q_SLOTS:
        int toggleMount();
        int mount();
        int umount();
        int remount();
        void setMountCommand(const QString & mnt);
        void setUmountCommand(const QString & umnt);
        void setDeviceName(const QString & deviceName);
        void setMountPoint(const QString & mountPoint);
        void setIconName(const QString & iconName);
        void setIconToDefault();
        void setMountOptions(const QString & mountOptions);
        void setFsType(const QString & fsType);
        void setMounted(bool nowMounted);
        void setKBSize(qulonglong kb_size);
        void setKBUsed(qulonglong kb_used);
        void setKBAvail(qulonglong kb_avail);
        QString guessIconName();
        
        private slots:
        void receivedSysStdErrOut();
    
    private:
        void init(const char *name);
        int sysCall(QString & command);
        QString prettyPrint(int kBValue) const;
        
        KProcess     *sysProc;
        QString           sysStringErrOut;
        bool              readingSysStdErrOut;
        
        QString     device,
        type,
        mountedOn,
        options,
        icoName,
        mntcmd,
        umntcmd;
        
        qulonglong  size,
        used,
        avail;       // ATTENTION: used+avail != size (clustersize!)
        
        bool        isMounted,
        iconSetByUser;
};

#endif

