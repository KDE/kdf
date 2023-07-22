/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//

#include "kdfconfig.h"

#include <KConfigGroup>

#include <QCheckBox>
#include <QCloseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif


KDFConfigWidget::KDFConfigWidget(QWidget *parent, bool init)
        : QWidget( parent)
{

    m_columnList.append( Column( QStringLiteral( "Icon" ), i18nc("TODO", "Icon"), 20, KDFWidget::IconCol) );
    m_columnList.append( Column( QStringLiteral( "Device" ), i18nc("Device of the storage", "Device"), 100, KDFWidget::DeviceCol) );
    m_columnList.append( Column( QStringLiteral( "Type" ), i18nc("Filesystem on storage", "Type"), 100 , KDFWidget::TypeCol) );
    m_columnList.append( Column( QStringLiteral( "Size" ), i18nc("Total size of the storage", "Size"), 100, KDFWidget::SizeCol) );
    m_columnList.append( Column( QStringLiteral( "MountPoint" ), i18nc("Mount point of the storage", "Mount Point"), 100, KDFWidget::MountPointCol) );
    m_columnList.append( Column( QStringLiteral( "Free" ), i18nc("Free space in storage", "Free"), 100, KDFWidget::FreeCol) );
    m_columnList.append( Column( QStringLiteral( "Full%" ), i18nc("Used storage space in %", "Full %"), 50, KDFWidget::FullCol ));
    m_columnList.append( Column( QStringLiteral( "UsageBar" ), i18nc("Usage graphical bar", "Usage"), 100, KDFWidget::UsageBarCol) );

    GUI = !init;
    if( GUI )
    {
        setupUi(this);

        QStringList headerLabels;
        for ( const Column &c : std::as_const(m_columnList))
        {
            headerLabels << c.columnName;
        }
        m_listWidget->setHeaderLabels(headerLabels);

        connect( m_listWidget, &QTreeWidget::itemClicked,
                 this, &KDFConfigWidget::toggleListText );
        connect( m_listWidget, &QTreeWidget::itemClicked,
                 this, &KDFConfigWidget::slotChanged );
        const int smallIcon = qApp->style()->pixelMetric(QStyle::PM_ToolBarIconSize);
        iconVisible = QIcon::fromTheme( QStringLiteral( "dialog-ok-apply" ) ).pixmap(smallIcon);
        iconHidden = QIcon::fromTheme( QStringLiteral( "dialog-cancel" ) ).pixmap(smallIcon);

        QTreeWidgetItem * item = new QTreeWidgetItem( m_listWidget );
        for( int i=0; i < m_columnList.size(); i++ )
        {
            item->setText( i, i18nc("Visible items on device information columns (enable|disable)", "visible") );
            item->setIcon( i, iconVisible );
            item->setData( i, Qt::UserRole, QVariant( true ) );
        }
        m_listWidget->setCurrentItem( item );

        connect(m_updateSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotChanged()) );

        connect(mFileManagerEdit,&QLineEdit::textChanged,this,&KDFConfigWidget::slotChanged);

        connect(mSystemFileManagerCheck,&QAbstractButton::toggled,this,&KDFConfigWidget::slotChanged);
        connect(mSystemFileManagerCheck,&QAbstractButton::toggled,mFileManagerEdit,&QWidget::setDisabled);
        connect(mSystemFileManagerCheck,&QAbstractButton::toggled,fileManagerEdit,&QWidget::setDisabled);

        connect(mOpenMountCheck,&QAbstractButton::toggled,this,&KDFConfigWidget::slotChanged);

        connect(mPopupFullCheck,&QAbstractButton::toggled,this,&KDFConfigWidget::slotChanged);

        if(!QStandardPaths::locate(QStandardPaths::RuntimeLocation, QLatin1String("flatpak-info")).isEmpty())
        {
            mFlatpakLabel->setVisible( true );
            mSystemFileManagerCheck->setEnabled( false );
            mFileManagerEdit->setEnabled( false );
        }
    }

    loadSettings();
    if( init )
        applySettings();

}


KDFConfigWidget::~KDFConfigWidget()
{
    delete m_listWidget;
}


void KDFConfigWidget::closeEvent(QCloseEvent *)
{
    applySettings();
    qApp->quit();
}


void KDFConfigWidget::applySettings( void )
{
    KConfigGroup config(KSharedConfig::openConfig(), "KDFConfig");

    if( GUI )
    {
        //config.writeEntry( "Width", width() );
        //config.writeEntry( "Height", height() );

        mStd.setFileManager( mFileManagerEdit->text() );
        mStd.setUpdateFrequency( m_updateSpinBox->value() );
        mStd.setPopupIfFull( mPopupFullCheck->isChecked() );
        mStd.setOpenFileManager( mOpenMountCheck->isChecked() );
        mStd.setUseSystemFileManager( mSystemFileManagerCheck->isChecked() );
        mStd.writeConfiguration();

        QTreeWidgetItem * item = m_listWidget->topLevelItem(0);
        for( int i=0; i < m_columnList.size(); i++ )
        {
            bool visible = item->data( i, Qt::UserRole ).toBool();
            config.writeEntry( m_columnList.at(i).name , visible );
        }

    }
    else
    {
        mStd.writeDefaultFileManager();
    }
    config.sync();
}


void KDFConfigWidget::loadSettings( void )
{
    KConfigGroup config(KSharedConfig::openConfig(), "KDFConfig");

    if( GUI )
    {
        mStd.updateConfiguration();
        m_updateSpinBox->setValue( mStd.updateFrequency() );
        mPopupFullCheck->setChecked( mStd.popupIfFull() );
        mOpenMountCheck->setChecked( mStd.openFileManager() );
        mSystemFileManagerCheck->setChecked( mStd.useSystemFileManager() );
        mFileManagerEdit->setText( mStd.fileManager() );

        mFileManagerEdit->setEnabled( !mStd.useSystemFileManager() );
        fileManagerEdit->setEnabled( !mStd.useSystemFileManager() );

        QTreeWidgetItem * item = m_listWidget->topLevelItem(0);
        for( int i=0; i < m_columnList.size(); i++ )
        {
            bool visible = config.readEntry( m_columnList.at(i).name, true );

            item->setText( i, visible ? i18nc("Are items on device information columns visible?", "visible") :
                           i18nc("Are items on device information columns hidden?", "hidden") );
            item->setIcon( i, visible ? iconVisible : iconHidden );
            item->setData( i, Qt::UserRole, QVariant( visible ) );
        }
    }

}

void KDFConfigWidget::defaultsBtnClicked( void )
{
    mStd.setDefault();
    m_updateSpinBox->setValue( mStd.updateFrequency() );
    mPopupFullCheck->setChecked( mStd.popupIfFull() );
    mOpenMountCheck->setChecked( mStd.openFileManager() );
    mSystemFileManagerCheck->setChecked( mStd.useSystemFileManager() );
    mFileManagerEdit->setText( mStd.fileManager() );

    QTreeWidgetItem * item = m_listWidget->topLevelItem(0);
    for( int i=0; i < m_columnList.size(); i++ )
    {
        item->setText( i, i18nc("Visible items on device information columns (enable|disable)", "visible") );
        item->setIcon( i, iconVisible );
        item->setData( i, Qt::UserRole, QVariant( true ) );
    }
    m_listWidget->setCurrentItem( item );
}


void KDFConfigWidget::toggleListText( QTreeWidgetItem *item, int column )
{
    if (!item)
        return;
    QString text = item->text( column );

    bool visible = item->data( column, Qt::UserRole).toBool();
    item->setData( column, Qt::UserRole, !visible );

    item->setText(column, visible ? i18nc("Device information item is hidden", "hidden"):
                  i18nc("Device information item is visible", "visible") );
    item->setIcon( column, visible ? iconHidden : iconVisible );
}

void KDFConfigWidget::slotChanged()
{
    Q_EMIT configChanged();
}

#include "moc_kdfconfig.cpp"
