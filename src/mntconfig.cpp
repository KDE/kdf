/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//

#include "mntconfig.h"

#include <KMessageBox>
#include <KIconLoader>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QPixmap>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#ifndef CMAKE_UNITY_BUILD
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
        connect( &mDiskList, &DiskList::readDFDone, this, &MntConfigWidget::readDFDone);

        connect ( m_listWidget, &QTreeWidget::itemClicked , this, &MntConfigWidget::clicked );
        m_listWidget->setHeaderLabels( QStringList() << QLatin1String( "" ) << i18n("Device")
                                       << i18n("Mount Point") << i18n("Mount Command") << i18n("Unmount Command") );
        m_listWidget->setColumnWidth( 0, 20 );

        QString text = QStringLiteral("%1: %2  %3: %4").
               arg(i18n("Device")).
               arg(i18nc("No device is selected", "None")).
               arg(i18n("Mount Point")).
               arg(i18nc("No mount point is selected", "None"));

        mGroupBox->setEnabled( false );
        mGroupBox->setTitle(text);

        connect( mIconLineEdit, &QLineEdit::textEdited,
                 this,&MntConfigWidget::iconChanged);
        connect( mIconLineEdit, &QLineEdit::textEdited,
                 this,&MntConfigWidget::slotChanged);

        mIconButton->setIconType(KIconLoader::Small, KIconLoader::Device);
        mIconButton->setFixedHeight( mIconButton->sizeHint().height() );

        connect( mIconButton, &KIconButton::iconChanged, this, &MntConfigWidget::iconChangedButton);
        connect( mIconButton, &KIconButton::iconChanged, this, &MntConfigWidget::slotChanged);

        connect( mDefaultIconButton, &QAbstractButton::clicked, this, &MntConfigWidget::iconDefault );
        connect( mDefaultIconButton, &QAbstractButton::clicked, this, &MntConfigWidget::slotChanged );

        connect( mMountLineEdit,&QLineEdit::textChanged,
                this,&MntConfigWidget::mntCmdChanged);
        connect( mMountLineEdit, &QLineEdit::textChanged,
                this,&MntConfigWidget::slotChanged);

        connect( mMountButton, &QAbstractButton::clicked, this, &MntConfigWidget::selectMntFile );

        connect( mUmountLineEdit, &QLineEdit::textChanged,
                 this,&MntConfigWidget::umntCmdChanged);
        connect( mUmountLineEdit, &QLineEdit::textChanged,
                 this,&MntConfigWidget::slotChanged);

        connect( mUmountButton,&QAbstractButton::clicked,this,&MntConfigWidget::selectUmntFile);
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

    QTreeWidgetItem *item = nullptr;

    DisksConstIterator itr = mDiskList.disksConstIteratorBegin();
    DisksConstIterator end = mDiskList.disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;
        item = new QTreeWidgetItem( m_listWidget, QStringList() << QString() << disk->deviceName()
                                    << disk->mountPoint() << disk->mountCommand() << disk->umountCommand() );
        item->setIcon( IconCol, QIcon::fromTheme( disk->iconName() ) );
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
    if( !mInitializing && GUI )
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if( isTopLevel() )
#else
        if( isWindow() )
#endif
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
    mGroupBox->setTitle( QStringLiteral("%1: %2  %3: %4").
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

    const int smallIcon = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);
    const QPixmap icon = QIcon::fromTheme( iconName ).pixmap(smallIcon);
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
    QUrl url = QFileDialog::getOpenFileUrl( this, QStringLiteral( "*" ));

    if( url.isEmpty() )
        return;

    if( !url.isLocalFile() )
    {
        KMessageBox::error( nullptr, i18n( "Only local files supported." ) );
        return;
    }

    mMountLineEdit->setText( url.path() );
}

void MntConfigWidget::selectUmntFile()
{
    QUrl url = QFileDialog::getOpenFileUrl( this, QStringLiteral( "*" ));

    if( url.isEmpty() )
        return;

    if( !url.isLocalFile() )
    {
        KMessageBox::error( nullptr, i18n( "Only local files are currently supported." ) );
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
    if( item == nullptr )
    {
        QList<QTreeWidgetItem*> selected = m_listWidget->selectedItems();
        if ( selected.size() == 1 )
            item =  selected.at(0);
        else
            return nullptr;
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
    Q_EMIT configChanged();
}



#include "moc_mntconfig.cpp"
