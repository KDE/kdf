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

#include "disks.h"

#include "kdfprivate_export.h"

#include <ksharedconfig.h>

// defines the os-type
#include <qglobal.h>

static QLatin1String DF_Command = QLatin1String( "df" );

#if defined(Q_OS_LINUX)
    static QLatin1String DF_Args = QLatin1String( "-kT" );
    static const bool No_FS_Type = false;
#else
    static QLatin1String DF_Args = QLatin1String( "-k" );
    static const bool No_FS_Type = true;
#endif

#if defined(Q_OS_SOLARIS)
    static QLatin1String CacheFSTAB = QLatin1String( "/etc/cachefstab" );
    static QLatin1String FSTAB = QLatin1String( "/etc/vfstab" );
#else
    static QLatin1String FSTAB = QLatin1String( "/etc/fstab" );
#endif

static const QLatin1Char Separator = QLatin1Char( '|' );

/***************************************************************************/
typedef QList<DiskEntry*>		                 Disks;
typedef QList<DiskEntry*>::const_iterator		 DisksConstIterator;
typedef QList<DiskEntry*>::iterator		         DisksIterator;

class KProcess;

class KDFPRIVATE_EXPORT DiskList : public QObject
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

    public Q_SLOTS:
        void loadSettings();
        void applySettings();

    private Q_SLOTS:
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

