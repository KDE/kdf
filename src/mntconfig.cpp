/*
 * mntconfig.cpp
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
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

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//

#include "mntconfig.h"

#include <kicondialog.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kcmodule.h>
#include <kiconloader.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <QGroupBox>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#ifndef KDE_USE_FINAL
static bool GUI;
#endif

MntConfigWidget::MntConfigWidget(QWidget *parent, bool init)
        : QWidget(parent)
{
    mInitializing = false;

    GUI = !init;
    if (GUI)
    {
        setupUi(this);

        //tabList fillup waits until disklist.readDF() is done...
        mDiskList.readFSTAB();
        mDiskList.readDF();
        mInitializing = true;
        connect( &mDiskList, SIGNAL(readDFDone()), this, SLOT(readDFDone()));

        connect ( m_listWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)) , this, SLOT(clicked(QTreeWidgetItem*,int)) );
        m_listWidget->setHeaderLabels( QStringList() << QLatin1String( "" ) << i18n("Device")
                                       << i18n("Mount Point") << i18n("Mount Command") << i18n("Unmount Command") );
        m_listWidget->setColumnWidth( 0, 20 );

        QString text = QString::fromLatin1("%1: %2  %3: %4").
               arg(i18n("Device")).
               arg(i18nc("No device is selected", "None")).
               arg(i18n("Mount Point")).
               arg(i18nc("No mount point is selected", "None"));

        mGroupBox->setEnabled( false );
        mGroupBox->setTitle(text);

        connect( mIconLineEdit, SIGNAL(textEdited(QString)),
                 this,SLOT(iconChanged(QString)));
        connect( mIconLineEdit, SIGNAL(textEdited(QString)),
                 this,SLOT(slotChanged()));

        mIconButton->setIconType(KIconLoader::Small, KIconLoader::Device);
        mIconButton->setFixedHeight( mIconButton->sizeHint().height() );

        connect( mIconButton, SIGNAL(iconChanged(QString)), this, SLOT(iconChangedButton(QString)));
        connect( mIconButton, SIGNAL(iconChanged(QString)), this, SLOT(slotChanged()));

        connect( mDefaultIconButton, SIGNAL(clicked()), this, SLOT(iconDefault()) );
        connect( mDefaultIconButton, SIGNAL(clicked()), this, SLOT(slotChanged()) );

        connect( mMountLineEdit,SIGNAL(textChanged(QString)),
                this,SLOT(mntCmdChanged(QString)));
        connect( mMountLineEdit, SIGNAL(textChanged(QString)),
                this,SLOT(slotChanged()));

        connect( mMountButton, SIGNAL(clicked()), this, SLOT(selectMntFile()) );

        connect( mUmountLineEdit, SIGNAL(textChanged(QString)),
                 this,SLOT(umntCmdChanged(QString)));
        connect( mUmountLineEdit, SIGNAL(textChanged(QString)),
                 this,SLOT(slotChanged()));

        connect( mUmountButton,SIGNAL(clicked()),this,SLOT(selectUmntFile()));
    }

    loadSettings();
    if(init)
    {
        applySettings();
    }
}


MntConfigWidget::~MntConfigWidget( void )
{
    delete m_listWidget;
}


void MntConfigWidget::readDFDone( void )
{
    mInitializing = false;
    m_listWidget->clear();

    QTreeWidgetItem *item = 0;

    DisksConstIterator itr = mDiskList.disksConstIteratorBegin();
    DisksConstIterator end = mDiskList.disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;
        item = new QTreeWidgetItem( m_listWidget, QStringList() << QString() << disk->deviceName()
                                    << disk->mountPoint() << disk->mountCommand() << disk->umountCommand() );
        item->setIcon( IconCol, SmallIcon( disk->iconName() ) );
    }

    /*
    //Adjust dialog size (?)
    m_listWidget->resizeColumnToContents( 2 );
    m_listWidget->resize(m_listWidget->sizeHint());
    m_listWidget->adjustSize();
    */

    loadSettings();
    applySettings();
}


void MntConfigWidget::applySettings( void )
{
    mDiskList.applySettings();

    KConfigGroup config(KSharedConfig::openConfig(), "MntConfig");
    if( GUI )
    {
        config.writeEntry("Width", width() );
        config.writeEntry("Height", height() );
    }
    config.sync();
}


void MntConfigWidget::loadSettings( void )
{
    KConfigGroup config = KSharedConfig::openConfig()->group("MntConfig");
    if( mInitializing == false && GUI )
    {
        if( isTopLevel() )
        {
            int w = config.readEntry("Width",this->width() );
            int h = config.readEntry("Height",this->height() );
            resize(w,h);
        }

        QList<QTreeWidgetItem*> list = m_listWidget->selectedItems();
        if( list.size() == 1 )
            clicked( list.at(0), 0 );
    }
}


void MntConfigWidget::clicked( QTreeWidgetItem * item , int col )
{
    Q_UNUSED(col);

    QTreeWidgetItem * header = m_listWidget->headerItem();

    mGroupBox->setEnabled( true );
    mGroupBox->setTitle( QString::fromLatin1("%1: %2  %3: %4").
                         arg(header->text( DeviceCol )).
                         arg(item->text( DeviceCol )).
                         arg(header->text( MountPointCol )).
                         arg(item->text( MountPointCol )) );


    const QIcon icon = item->icon( IconCol );
    if( !icon.isNull() )
        mIconButton->setIcon( icon );

    DiskEntry * disk = selectedDisk( item );
    if (!disk)
        return;

    mIconLineEdit->setText( disk->iconName() );

    mMountLineEdit->setText( item->text( MountCommandCol ) );
    mUmountLineEdit->setText( item->text( UmountCommandCol ) );
}


void MntConfigWidget::iconChangedButton(const QString &iconName)
{
    iconChanged(iconName);
}

void MntConfigWidget::iconChanged(const QString &iconName)
{
    QList<QTreeWidgetItem*> list = m_listWidget->selectedItems();
    QTreeWidgetItem * item = list.at(0);

    DiskEntry * disk = selectedDisk( item );
    if ( !disk )
        return;

    disk->setIconName(iconName);
    mIconLineEdit->setText(iconName);

    QPixmap icon = SmallIcon( iconName );
    item->setIcon( IconCol, icon );
    mIconButton->setIcon( icon );

    slotChanged();
}

void MntConfigWidget::iconDefault()
{
    QList<QTreeWidgetItem*> list = m_listWidget->selectedItems();
    QTreeWidgetItem * item = list.at(0);

    DiskEntry * disk = selectedDisk( item );
    if ( !disk )
        return;

    iconChanged(disk->guessIconName());
}

void MntConfigWidget::selectMntFile()
{
    QUrl url = QFileDialog::getOpenFileUrl( this, QLatin1String( "*" ));

    if( url.isEmpty() )
        return;

    if( !url.isLocalFile() )
    {
        KMessageBox::sorry( 0L, i18n( "Only local files supported." ) );
        return;
    }

    mMountLineEdit->setText( url.path() );
}

void MntConfigWidget::selectUmntFile()
{
    QUrl url = QFileDialog::getOpenFileUrl( this, QLatin1String( "*" ));

    if( url.isEmpty() )
        return;

    if( !url.isLocalFile() )
    {
        KMessageBox::sorry( 0L, i18n( "Only local files are currently supported." ) );
        return;
    }

    mUmountLineEdit->setText( url.path() );
}

void MntConfigWidget::mntCmdChanged( const QString &data )
{

    QList<QTreeWidgetItem*> list = m_listWidget->selectedItems();
    QTreeWidgetItem * item = list.at(0);

    DiskEntry * disk = selectedDisk( item );
    if ( !disk )
        return;

    disk->setMountCommand( data );
    item->setText( MountCommandCol , data );

}


void MntConfigWidget::umntCmdChanged( const QString &data )
{
    QList<QTreeWidgetItem*> list = m_listWidget->selectedItems();
    QTreeWidgetItem * item = list.at(0);

    DiskEntry * disk = selectedDisk( item );
    if ( !disk )
        return;

    disk->setUmountCommand( data );
    item->setText( UmountCommandCol , data );

}

DiskEntry * MntConfigWidget::selectedDisk( QTreeWidgetItem * item )
{
    if( item == 0 )
    {
        QList<QTreeWidgetItem*> selected = m_listWidget->selectedItems();
        if ( selected.size() == 1 )
            item =  selected.at(0);
        else
            return 0;
    }

    DiskEntry * tmpDisk = new DiskEntry(item->text( DeviceCol ));
    tmpDisk->setMountPoint(item->text( MountPointCol ));

    int pos = mDiskList.find(tmpDisk);

    delete tmpDisk;

    return mDiskList.at(pos);
}

void MntConfigWidget::closeEvent(QCloseEvent *)
{
}

void MntConfigWidget::slotChanged()
{
    emit configChanged();
}


