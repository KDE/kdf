/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef DISKLIST_H
#define DISKLIST_H

#include "disks.h"

#include "kdfprivate_export.h"

#include <KSharedConfig>

// defines the os-type
#include <qglobal.h>

static QLatin1String DF_Command = QLatin1String( "df" );

#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD)
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
        explicit DiskList( QObject *parent=nullptr );
        ~DiskList() override;

        int readFSTAB();
        int readDF();
        int find(DiskEntry* disk);
        DiskEntry* at(int index) { return disks->at(index); }
        int count() const { return disks->count(); }
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

