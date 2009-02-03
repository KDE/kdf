/*
* kdfwidget.h
*
* Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
*               2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

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

#ifndef __KDFWIDGET_H__
#define __KDFWIDGET_H__

#include "disks.h"
#include "disklist.h"
#include "mntconfig.h"
#include "kdfconfig.h"
#include "stdoption.h"

#include "kdfitemdelegate.h"
#include "kdfsortproxymodel.h"

class QTreeView;
//class QTreeWidgetItem;
class QTimer;
class COptionDialog;
class KMenu;

class QStandardItemModel;

/* Column class (id, column number, column name) */
class Column
{
    public:
    
    Column(QString _name, QString _columnName, int _defaultWidth, int _number)
    {
        name = _name;
        columnName = _columnName;
        defaultWidth = _defaultWidth;
        number = _number;
    }
    
    QString name;
    QString columnName;
    int defaultWidth;
    int number;
    
};

class KDFWidget : public QWidget
{
    Q_OBJECT
    
    public:
        explicit KDFWidget( QWidget *parent=0, bool init=false);
        ~KDFWidget( );
        
        enum ColumnNumber{
            IconCol = 0,
            DeviceCol = 1,
            TypeCol = 2,
            SizeCol = 3,
            MountPointCol = 4,
            FreeCol = 5,
            FullCol = 6,
            UsageBarCol = 7
        };
    
    public Q_SLOTS:
        void settingsChanged( void );
        void loadSettings( void );
        void applySettings( void );
        void updateDF( void );
        void updateDFDone( void );
        void settingsBtnClicked( void );
    
    private Q_SLOTS:
        void criticallyFull( DiskEntry *disk );
        void contextMenuRequested ( const QPoint &p );
        void setUpdateFrequency( int frequency );
        void invokeHelp( void );
    
    protected:
        void timerEvent( QTimerEvent * );
        void closeEvent( QCloseEvent * );
    
    private:
        void makeColumns( void );
        DiskEntry *selectedDisk( QModelIndex index );
        QIcon generateIcon( QString iconName , bool mode, bool mounted);
    
        bool readingDF;
        COptionDialog *mOptionDialog;
        KMenu    *mPopup;
        QTimer        *mTimer;
        DiskList   mDiskList;
        bool       mIsTopLevel;
        CStdOption mStd;
        
        QTreeView * m_listWidget;
        KDFItemDelegate * m_itemDelegate;
        QList<Column> m_columnList;
        
        QStandardItemModel * m_listModel;
        KDFSortFilterProxyModel * m_sortModel;
};

#endif

