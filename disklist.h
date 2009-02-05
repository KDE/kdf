/*
* disklist.h
*
* Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
*               2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>
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

#ifndef DISKLIST_H
#define DISKLIST_H

// defines the os-type
#include <QtCore/qglobal.h>

#include <ksharedconfig.h>

#include "disks.h"

static const char DF_Command[] = "df";

#if defined(Q_OS_LINUX)
    static const char DF_Args[] = "-kT";
    static const bool No_FS_Type = false;
#else
    static const char DF_Args[] = "-k";
    static const bool No_FS_Type = true;
#endif

#if defined(Q_OS_SOLARIS)
    static const char CacheFSTAB[] = "/etc/cachefstab";
    static const char FSTAB[] = "/etc/vfstab";
#else
    static const char FSTAB[] = "/etc/fstab";
#endif

static const char Separator[] = "|";

/***************************************************************************/
typedef QList<DiskEntry*>		                 Disks;
typedef QList<DiskEntry*>::const_iterator		 DisksConstIterator;
typedef QList<DiskEntry*>::iterator		         DisksIterator;

class KProcess;

class DiskList : public QObject
{  
    Q_OBJECT
    
    public:
        DiskList( QObject *parent=0 );
        ~DiskList();
        
        int readFSTAB();
        int readDF();
        int find(DiskEntry* disk);
        DiskEntry* at(uint index) { return disks->at(index); }
        uint count() { return disks->count(); }
        void deleteAllMountedAt(const QString &mountpoint);
        void setUpdatesDisabled(bool disable);
        
        //To iterate over disks items
        DisksConstIterator disksConstIteratorBegin() { return disks->constBegin(); }
        DisksConstIterator disksConstIteratorEnd() { return disks->constEnd(); }
        
        DisksIterator disksIteratorBegin() { return disks->begin(); }
        DisksIterator disksIteratorEnd() { return disks->end(); }

    Q_SIGNALS:
        void readDFDone();
        void criticallyFull(DiskEntry *disk);
        
        public slots:
        void loadSettings();
        void applySettings();
        
        private slots:
        void dfDone();

    private:
        void replaceDeviceEntry(DiskEntry *disk);
        
        Disks  *disks;
        KProcess         *dfProc;
        bool              readingDFStdErrOut;
        KSharedConfigPtr  config;
        bool              updatesDisabled;

};
/***************************************************************************/


#endif

