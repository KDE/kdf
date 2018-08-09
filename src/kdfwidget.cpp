/*
 * kdfwidget.cpp
 *
 * Copyright (c) 1998-2001 Michael Kropfberger <michael.kropfberger@gmx.net>
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

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
// 1999-12-05 Espen Sand
// Usage bars should work again.
//

#include "kdfwidget.h"

#include "kdfprivate_debug.h"
#include "optiondialog.h"

#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kshell.h>
#include <khelpclient.h>
#include <kconfiggroup.h>

#include <QAbstractEventDispatcher>
#include <QTimer>
#include <QFile>
#include <QLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPainter>
#include <QMenu>
#include <QProcess>

//This aren't used here...
//#define BAR_COLUMN   7
//#define FULL_PERCENT 95.0

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif

KDFWidget::KDFWidget( QWidget *parent, bool init )
        : QWidget(parent), mOptionDialog(nullptr), mPopup(nullptr), mTimer(nullptr)
{
    connect(&mDiskList , SIGNAL(readDFDone()),
            this, SLOT (updateDFDone()) );
    connect(&mDiskList , SIGNAL(criticallyFull(DiskEntry*)),
            this, SLOT (criticallyFull(DiskEntry*)) );

    m_columnList.append( Column( QLatin1String( "Icon" ), QLatin1String( "" ), 20, IconCol ));
    m_columnList.append( Column( QLatin1String( "Device" ), i18nc("Device of the storage", "Device"), 100, DeviceCol ));
    m_columnList.append( Column( QLatin1String( "Type" ), i18nc("Filesystem on storage", "Type"), 80, TypeCol ));
    m_columnList.append( Column( QLatin1String( "Size" ), i18nc("Total size of the storage", "Size"), 80, SizeCol ));
    m_columnList.append( Column( QLatin1String( "MountPoint" ), i18nc("Mount point of storage", "Mount Point"), 120, MountPointCol ));
    m_columnList.append( Column( QLatin1String( "Free" ), i18nc("Free space in storage", "Free"), 80, FreeCol ));
    m_columnList.append( Column( QLatin1String( "Full%" ), i18nc("Used storage space in %", "Full %"), 50, FullCol ));
    m_columnList.append( Column( QLatin1String( "UsageBar" ), i18nc("Usage graphical bar", "Usage"), 200, UsageBarCol ));

    GUI = !init;
    if( GUI )
    {
        QVBoxLayout *topLayout = new QVBoxLayout( this );
        topLayout->setSpacing( 0 );
        topLayout->setMargin( 0 );

        m_listModel = new QStandardItemModel( this );
        m_sortModel = new KDFSortFilterProxyModel( this );
        m_sortModel->setSourceModel( m_listModel );

        m_listWidget = new QTreeView( this );
        m_listWidget->setModel( m_sortModel );

        m_itemDelegate = new KDFItemDelegate( m_listWidget );

        m_listWidget->setItemDelegate( m_itemDelegate );
        m_listWidget->setRootIsDecorated( false );
        m_listWidget->setSortingEnabled( true );
        m_listWidget->setContextMenuPolicy( Qt::CustomContextMenu );
        m_listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        topLayout->addWidget( m_listWidget );

        connect( m_listWidget, SIGNAL(customContextMenuRequested(QPoint)),
                 this, SLOT(contextMenuRequested(QPoint)));

        makeColumns();

        mIsTopLevel = QLatin1String(parent->metaObject()->className()) == QLatin1String( "KDFTopLevel" ) ? true : false;
    }

    loadSettings();
    if( init )
        applySettings();
}

KDFWidget::~KDFWidget()
{
    delete m_listModel;
    delete m_sortModel;
    delete m_itemDelegate;
    delete m_listWidget;
}

void KDFWidget::makeColumns( void )
{

    QStringList columns;
    Q_FOREACH(const Column &c, m_columnList){
        columns << c.columnName;
    }
    m_listModel->setHorizontalHeaderLabels( columns );

}

/******************************************************************/
void KDFWidget::closeEvent(QCloseEvent *)
{
    applySettings();
    qApp->quit();
}


void KDFWidget::settingsChanged( void )
{
    applySettings();
    loadSettings();
}


/***************************************************************************
  * writes the KConfig
**/
void KDFWidget::applySettings( void )
{
    KConfig m_config;
    KConfigGroup config( &m_config, "KDiskFree" );

    if( GUI )
    {
        Q_FOREACH(const Column &c, m_columnList){
            if( !m_listWidget->isColumnHidden( c.number ) )
                config.writeEntry( c.name, m_listWidget->columnWidth(c.number) );
        }

        config.writeEntry("SortColumn", m_sortModel->sortColumn());
        config.writeEntry("SortOrder", (int)m_sortModel->sortOrder());

        //Save the sort order of the QTreeView Header
        QHeaderView * header = m_listWidget->header();
        QList<int> sectionIndices;
        for (int i = 0; i < header->count(); i++) {
            sectionIndices.append(header->visualIndex(i));
        }
        config.writeEntry("HeaderSectionIndices", sectionIndices);
    }
    config.sync();
    updateDF();
}


/***************************************************************************
  * reads the KConfig
**/
void KDFWidget::loadSettings( void )
{
    mStd.updateConfiguration();

    if(GUI)
    {
        KConfigGroup config(KSharedConfig::openConfig(), "KDiskFree");
        Q_FOREACH(const Column &c, m_columnList){
            int width = config.readEntry( c.name, c.defaultWidth );
            m_listWidget->setColumnWidth( c.number, width );
        }

        KConfigGroup config_visible(KSharedConfig::openConfig(), "KDFConfig");
        Q_FOREACH(const Column &c, m_columnList){
            bool visible = config_visible.readEntry( c.name , true );
            m_listWidget->setColumnHidden( c.number, !visible );
        }

        int sortColumn = config.readEntry("SortColumn",0);
        int sortOrder = config.readEntry("SortOrder",(int)Qt::AscendingOrder);
        m_listWidget->sortByColumn(sortColumn,Qt::SortOrder(sortOrder));

        //Load the sort order of the QTreeView Header
        //This can also be achieved by header->saveState() and header->restoreState(...),
        //but this would not be "human-readable" any more...
        QHeaderView * header = m_listWidget->header();
        QList<int> sectionIndices;
        sectionIndices = config.readEntry("HeaderSectionIndices",sectionIndices);
        if (sectionIndices.count() == header->count()) {
            for (int i = 0; i < header->count(); i++) {
                int sectionIndex = sectionIndices.at(i);
                int oldVisualIndex = header->visualIndex(sectionIndex);
                header->moveSection(oldVisualIndex,i);
            }
        }

        setUpdateFrequency( mStd.updateFrequency() );
        updateDF();
    }
}


/***************************************************************************
  * pops up the SettingsBox if the settingsBtn is clicked
**/
void KDFWidget::settingsBtnClicked( void )
{
    if( mIsTopLevel == true )
    {
        if( mOptionDialog == nullptr )
        {
            mOptionDialog = new COptionDialog( this );
            if( mOptionDialog == nullptr )
            {
                return;
            }
            connect( mOptionDialog, SIGNAL(valueChanged()),
                     this, SLOT(settingsChanged()) );
        }
        mOptionDialog->show();
    }
}


/***************************************************************************
  * resets the timer for automatic df-refreshes
**/
void KDFWidget::setUpdateFrequency( int frequency )
{
    //
    // Kill current timer and restart it if the frequency is
    // larger than zero.
    //
    QAbstractEventDispatcher::instance()->unregisterTimers(this);
    if( frequency > 0 )
    {
        startTimer( frequency * 1000 );
    }
}

/***************************************************************************
  * Update (reread) all disk-dependencies
**/
void KDFWidget::timerEvent(QTimerEvent *)
{
    updateDF();
}


/***************************************************************************
  * checks fstab & df
**/
void KDFWidget::updateDF( void )
{
    //
    // We can only do this if the popupmenu is not present
    //
    if( mPopup == nullptr )
    {
        readingDF = true;
        mDiskList.readFSTAB();
        mDiskList.readDF();
    }
}

/***************************************************************************
  * gets the signal when the diskList is complete and up to date
**/
void KDFWidget::updateDFDone( void ){
    if (mPopup) //The popup menu is ont he screen... Don't touch the list view...
        return;

    //Clear the list items
    m_listModel->removeRows(0,m_listModel->rowCount());

    DisksConstIterator itr = mDiskList.disksConstIteratorBegin();
    DisksConstIterator end = mDiskList.disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;

        QString size,percent;
        if( disk->kBSize() > 0 )
        {
            percent = i18nc("Disk percentage", "%1%", QString::number(disk->percentFull()));
            size = disk->prettyKBSize();
        }
        else
        {
            percent = i18n("N/A");
            size = i18n("N/A");
        }

        bool root = !disk->mountOptions().contains(QLatin1String( "user" ), Qt::CaseInsensitive);

        QStandardItem * IconItem = new QStandardItem( generateIcon(disk->iconName(), root, disk->mounted() ), QLatin1String( "" ) );

        QStandardItem * DeviceItem = new QStandardItem( disk->deviceName() );

        QStandardItem * TypeItem = new QStandardItem( disk->fsType() );

        QStandardItem * SizeItem = new QStandardItem( size );
        SizeItem->setData( disk->kBSize(), Qt::UserRole );

        QStandardItem * MountPointItem = new QStandardItem( disk->mountPoint() );

        QStandardItem * FreeItem = new QStandardItem( disk->prettyKBAvail() );
        FreeItem->setData( disk->kBAvail(), Qt::UserRole );

        QStandardItem * FullItem = new QStandardItem( percent );
        FullItem->setData( disk->percentFull() , Qt::UserRole );

        QStandardItem * UsageBarItem = new QStandardItem( QLatin1String( "" ) );
        UsageBarItem->setData( disk->percentFull(), Qt::UserRole );

        m_listModel->appendRow( QList<QStandardItem*>() << IconItem <<  DeviceItem << TypeItem << SizeItem << MountPointItem <<
          FreeItem << FullItem << UsageBarItem);
    }

    readingDF = false;

    m_listModel->sort( DeviceCol );

}

QIcon KDFWidget::generateIcon( QString iconName, bool mode, bool mounted)
{
    QPixmap pix = SmallIcon(iconName);

    QPainter painter(&pix);

    if( mode )
        painter.drawPixmap( QRect(0,6,10,10), SmallIcon(QLatin1String( "object-locked" )) );

    if( mounted )
        painter.drawPixmap( QRect(6,6,12,12) , SmallIcon(QLatin1String( "emblem-mounted" )) );

    painter.end();
    return QIcon(pix);

}

void KDFWidget::criticallyFull( DiskEntry *disk )
{
    if( mStd.popupIfFull() == true )
    {
        QString msg = i18n("Device [%1] on [%2] is critically full.",
                           disk->deviceName(), disk->mountPoint());
        KMessageBox::sorry( this, msg, i18nc("Warning device getting critically full", "Warning"));
    }
}

DiskEntry * KDFWidget::selectedDisk( QModelIndex index )
{
    if( !index.isValid() )
        return nullptr;

    QStandardItem * itemDevice = m_listModel->item( index.row() , DeviceCol );
    QStandardItem * itemMountPoint = m_listModel->item( index.row() , MountPointCol );

    DiskEntry * disk = new DiskEntry( itemDevice->text() );
    disk->setMountPoint( itemMountPoint->text() );

    int pos = mDiskList.find( disk );

    delete disk;
    return mDiskList.at(pos);

}

void KDFWidget::contextMenuRequested( const QPoint &p )
{
    if (mPopup) //The user may even be able to popup another menu while this open is active...
        return;

    QModelIndex index = m_listWidget->indexAt( p );

    if( !index.isValid() )
    {
        QList<QModelIndex> selected = m_listWidget->selectionModel()->selectedIndexes();
        if ( selected.size() > 0 )
            index = selected.at(0);
        else
            return;
    }

    index = m_sortModel->mapToSource( index );

    mDiskList.setUpdatesDisabled(true);
    DiskEntry * disk = selectedDisk( index );

    if( disk == nullptr )
        return;

    mPopup = new QMenu( nullptr );
    mPopup->setTitle( disk->mountPoint() );
    QAction *mountPointAction = mPopup->addAction( i18n("Mount Device") );
    QAction *umountPointAction = mPopup->addAction( i18n("Unmount Device") );
    mPopup->addSeparator();
    QAction *openFileManagerAction = mPopup->addAction( i18n("Open in File Manager") );
    mountPointAction->setEnabled( !disk->mounted() );
    umountPointAction->setEnabled( disk->mounted() );
    openFileManagerAction->setEnabled( disk->mounted() );
    QAction *position = mPopup->exec( m_listWidget->mapToGlobal(p) );

    bool openFileManager = false;
    if( !position )
    {
        mDiskList.setUpdatesDisabled(false);
        delete mPopup;
        mPopup = nullptr;
        return;
    }
    else if( position == mountPointAction || position == umountPointAction )
    {
        QStandardItem * SizeItem = m_listModel->item( index.row() , SizeCol );
        SizeItem->setText( i18n("MOUNTING") );

        QStandardItem * FreeItem = m_listModel->item( index.row() , FreeCol );
        FreeItem->setText( i18n("MOUNTING") );

        QStandardItem * IconItem = m_listModel->item( index.row() , IconCol );
        IconItem->setIcon( SmallIcon(QLatin1String( "user-away" )) );

        int val = disk->toggleMount();
        if( val != 0 /*== false*/ )
        {
            KMessageBox::error( this, disk->lastSysError() );
        }
        else if ( ( mStd.openFileManager() == true)
                  && (position == mountPointAction) ) //only on mount
        {
            openFileManager = true;
        }

        //delete item;
        mDiskList.deleteAllMountedAt(disk->mountPoint());
    }
    else if( position == openFileManagerAction )
    {
        openFileManager = true;
    }

    if( openFileManager == true )
    {
        qCDebug(KDF) << "opening filemanager";
        if(  mStd.fileManager().isEmpty() == false )
        {
            QString cmd = mStd.fileManager();
            int pos = cmd.indexOf(QLatin1String( "%m" ));
            if( pos > 0 )
            {
                cmd = cmd.replace(pos, 2, KShell::quoteArg(disk->mountPoint()));
            }
            else
            {
                cmd += QLatin1Char( ' ' ) + KShell::quoteArg(disk->mountPoint());
            }
            QProcess::startDetached(cmd);
        }
    }

    //Update only here as showing of error message triggers event loop.
    mDiskList.setUpdatesDisabled(false);
    delete mPopup;
    mPopup = nullptr;

    if( position != openFileManagerAction ) // No need to update when just opening the fm.
    {
        updateDF();
    }

}

void KDFWidget::invokeHelp()
{
    KHelpClient::invokeHelp(QLatin1String( "" ), QLatin1String( "kcontrol/kdf" ));
}

