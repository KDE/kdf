/*
* mntconfig.h
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

#ifndef MNTCONFIG_H
#define MNTCONFIG_H

#include "ui_mntconfig.h"
#include "disklist.h"

#include <QWidget>

/**************************************************************/

class MntConfigWidget : public QWidget, private Ui_MntConfigWidget
{
    Q_OBJECT

    public:
        enum ColType
        {
            IconCol = 0,
            DeviceCol = 1,
            MountPointCol = 2,
            MountCommandCol = 3,
            UmountCommandCol = 4
        };

        explicit MntConfigWidget( QWidget *parent=nullptr, bool init=false );
        ~MntConfigWidget() override;

    public Q_SLOTS:
        void loadSettings( void );
        void applySettings( void );

    protected Q_SLOTS:
        void slotChanged();

    private Q_SLOTS:
        void readDFDone( void );
        void clicked( QTreeWidgetItem *, int);
        void selectMntFile( void );
        void selectUmntFile( void );
        void iconChangedButton( const QString & );
        void iconChanged( const QString & );
        void iconDefault();
        void mntCmdChanged( const QString & );
        void umntCmdChanged( const QString & );

        DiskEntry * selectedDisk( QTreeWidgetItem * );

    protected:
        void closeEvent( QCloseEvent * ) override;

    private:
        DiskList    mDiskList;
        bool        mInitializing;

    Q_SIGNALS:
        void configChanged();
};

#endif

