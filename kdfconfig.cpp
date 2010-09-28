/*
 * kdfconfig.cpp
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

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//

#include "kdfconfig.h"

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QLCDNumber>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCloseEvent>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QSizePolicy>

#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klineedit.h>

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif


KDFConfigWidget::KDFConfigWidget(QWidget *parent, bool init)
        : QWidget( parent)
{

    m_columnList.append( Column( QLatin1String( "Icon" ), i18nc("TODO", "Icon"), 20, KDFWidget::IconCol) );
    m_columnList.append( Column( QLatin1String( "Device" ), i18nc("Device of the storage", "Device"), 100, KDFWidget::DeviceCol) );
    m_columnList.append( Column( QLatin1String( "Type" ), i18nc("Filesystem on storage", "Type"), 100 , KDFWidget::TypeCol) );
    m_columnList.append( Column( QLatin1String( "Size" ), i18nc("Total size of the storage", "Size"), 100, KDFWidget::SizeCol) );
    m_columnList.append( Column( QLatin1String( "MountPoint" ), i18nc("Mount point of the storage", "Mount Point"), 100, KDFWidget::MountPointCol) );
    m_columnList.append( Column( QLatin1String( "Free" ), i18nc("Free space in storage", "Free"), 100, KDFWidget::FreeCol) );
    m_columnList.append( Column( QLatin1String( "Full%" ), i18nc("Used storage space in %", "Full %"), 50, KDFWidget::FullCol ));
    m_columnList.append( Column( QLatin1String( "UsageBar" ), i18nc("Usage graphical bar", "Usage"), 100, KDFWidget::UsageBarCol) );

    GUI = !init;
    if( GUI )
    {
        setupUi(this);

        QStringList headerLabels;
        Q_FOREACH( const Column &c, m_columnList)
        {
            headerLabels << c.columnName;
        }
        m_listWidget->setHeaderLabels(headerLabels);

        connect( m_listWidget, SIGNAL( itemClicked( QTreeWidgetItem*,int ) ),
                 this, SLOT( toggleListText( QTreeWidgetItem * , int )) );
        connect( m_listWidget, SIGNAL( itemClicked( QTreeWidgetItem*,int ) ),
                 this, SLOT( slotChanged() ) );

        iconVisible = MainBarIcon( QLatin1String( "dialog-ok-apply" ) );
        iconHidden = MainBarIcon( QLatin1String( "dialog-cancel" ) );

        QTreeWidgetItem * item = new QTreeWidgetItem( m_listWidget );
        for( int i=0; i < m_columnList.size(); i++ )
        {
            item->setText( i, i18nc("Visible items on device information columns (enable|disable)", "visible") );
            item->setIcon( i, iconVisible );
            item->setData( i, Qt::UserRole, QVariant( true ) );
        }
        m_listWidget->setCurrentItem( item );

        connect(m_updateSpinBox, SIGNAL( valueChanged(int) ), this, SLOT( slotChanged() ) );

        connect(mFileManagerEdit,SIGNAL(textChanged (const QString &)),this,SLOT(slotChanged()));

        connect(mOpenMountCheck,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));

        connect(mPopupFullCheck,SIGNAL(toggled(bool)),this,SLOT(slotChanged()));
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
    kapp->quit();
}


void KDFConfigWidget::applySettings( void )
{
    KConfigGroup config(KGlobal::config(), "KDFConfig");

    if( GUI )
    {
        //config.writeEntry( "Width", width() );
        //config.writeEntry( "Height", height() );

        mStd.setFileManager( mFileManagerEdit->text() );
        mStd.setUpdateFrequency( m_updateSpinBox->value() );
        mStd.setPopupIfFull( mPopupFullCheck->isChecked() );
        mStd.setOpenFileManager( mOpenMountCheck->isChecked() );
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
    KConfigGroup config(KGlobal::config(), "KDFConfig");

    if( GUI )
    {
        mStd.updateConfiguration();
        m_updateSpinBox->setValue( mStd.updateFrequency() );
        mPopupFullCheck->setChecked( mStd.popupIfFull() );
        mOpenMountCheck->setChecked( mStd.openFileManager() );
        mFileManagerEdit->setText( mStd.fileManager() );

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
    emit configChanged();
}

#include "kdfconfig.moc"

