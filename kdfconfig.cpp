/*
 * kdfconfig.cpp
 *
 * $Id$
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//


#include <qcheckbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qlineedit.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>

#include "listview.h"
#include "kdfconfig.h"

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif


KDFConfigWidget::KDFConfigWidget(QWidget *parent, const char *name, bool init)
  : QWidget( parent, name)
{

  mTabName.resize(8);
  mTabName[0] = new CTabName( "Icon", i18n("Icon") );
  mTabName[1] = new CTabName( "Device", i18n("Device") );
  mTabName[2] = new CTabName( "Type", i18n("Type") );
  mTabName[3] = new CTabName( "Size", i18n("Size") );
  mTabName[4] = new CTabName( "MountPoint", i18n("Mount point") );
  mTabName[5] = new CTabName( "Free", i18n("Free") );
  mTabName[6] = new CTabName( "Full%", i18n("Full %") );
  mTabName[7] = new CTabName( "UsageBar", i18n("Usage") );

  GUI = init ? FALSE : TRUE;
  if(GUI)
  {
    QString text;
    QVBoxLayout *topLayout = new QVBoxLayout( this, 0, KDialog::spacingHint());

    mList = new CListView( this, "list", 1 );
    mList->setAllColumnsShowFocus(true);
    mList->setFrameStyle( QFrame::WinPanel + QFrame::Sunken );
    mList->header()->setMovingEnabled(false);
    for( uint i=0; i < mTabName.size(); i++ )
    {
      mList->addColumn( mTabName[i]->mName );
    }
    connect( mList, SIGNAL(clicked(QListViewItem *, const QPoint &, int)),
	     this, SLOT(toggleListText(QListViewItem *,const QPoint &,int)));
    topLayout->addWidget( mList );

    QListViewItem *mListItem = new QListViewItem( mList );
    for( uint i=mList->header()->count(); i>0; i-- )
    {
     mListItem->setText(i-1, i18n("visible") );
     mListItem->setPixmap( i-1, UserIcon ( "tick" ) );
    }
    mList->setSelected( mListItem, true );

    QGridLayout *gl = new QGridLayout( 2, 2 );
    topLayout->addLayout( gl );
    gl->setColStretch( 1, 10 );

    mScroll = new QScrollBar( this );
    Q_CHECK_PTR(mScroll);
    mScroll->setOrientation( QScrollBar::Horizontal );
    mScroll->setSteps(1,20);
    mScroll->setRange(0, 180 );
    gl->addWidget( mScroll, 1, 1 );

    mLCD = new QLCDNumber( this );
    Q_CHECK_PTR(mLCD);
    mLCD->setNumDigits( 3 );
    mLCD->setSegmentStyle(QLCDNumber::Filled);
    connect(mScroll,SIGNAL(valueChanged(int)),mLCD,SLOT(display(int)));
    gl->addMultiCellWidget( mLCD, 0, 1, 0, 0 );

    text = i18n("Update frequency [seconds]. The value 0 disables update" );
    QLabel *label = new QLabel( text, this );
    Q_CHECK_PTR(label);
    gl->addWidget( label, 0, 1 );


    label = new QLabel( i18n("FileManager (e.g. konsole -e mc %m)") ,this);
    Q_CHECK_PTR(label);
    topLayout->addWidget( label );

    mFileManagerEdit = new QLineEdit( this );
    Q_CHECK_PTR(mFileManagerEdit);
    topLayout->addWidget( mFileManagerEdit );

    text = i18n("Open filemanager automatically on mount");
    mOpenMountCheck = new QCheckBox(text, this );
    Q_CHECK_PTR(mOpenMountCheck);
    topLayout->addWidget( mOpenMountCheck );

    text = i18n("Pop up a window when a disk gets critically full");
    mPopupFullCheck = new QCheckBox( text, this );
    Q_CHECK_PTR(mPopupFullCheck);
    topLayout->addWidget( mPopupFullCheck );
  }

  loadSettings();
  if( init )
  {
    applySettings();
  }
}


KDFConfigWidget::~KDFConfigWidget()
{
}


void KDFConfigWidget::closeEvent(QCloseEvent *)
{
  applySettings();
  kapp->quit();
}


void KDFConfigWidget::applySettings( void )
{
  KConfig &config = *kapp->config();
  config.setGroup("KDFConfig");

  if( GUI )
  {
    //config.writeEntry( "Width", width() );
    //config.writeEntry( "Height", height() );

    mStd.setFileManager( mFileManagerEdit->text() );
    mStd.setUpdateFrequency( mScroll->value() );
    mStd.setPopupIfFull( mPopupFullCheck->isChecked() );
    mStd.setOpenFileManager( mOpenMountCheck->isChecked() );
    mStd.writeConfiguration();

    QListViewItem *item = mList->firstChild();
    if( item != 0 )
    {
      for( int i=mList->header()->count(); i>0; i-- )
      {
	bool state = item->text(i-1) == i18n("visible") ? true : false;
	config.writeEntry( mTabName[i-1]->mRes, state );
      }
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
  KConfig &config = *kapp->config();
  config.setGroup("KDFConfig");

  if( GUI )
  {
    mStd.updateConfiguration();
    mScroll->setValue( mStd.updateFrequency() );
    mLCD->display( mStd.updateFrequency() );
    mPopupFullCheck->setChecked( mStd.popupIfFull() );
    mOpenMountCheck->setChecked( mStd.openFileManager() );
    mFileManagerEdit->setText( mStd.fileManager() );

    QListViewItem *item = mList->firstChild();
    if( item != 0 )
    {
      for( int i=mList->header()->count(); i>0; i-- )
      {
	int j = config.readNumEntry( mTabName[i-1]->mRes, 1 );
	item->setText( i-1, j==0 ? i18n("hidden") : i18n("visible") );
     	item->setPixmap( i-1, j==0 ? UserIcon ( "delete" )
                                   : UserIcon ( "tick" ) );
      }
    }
  }

}

void KDFConfigWidget::defaultsBtnClicked( void )
{
  mStd.setDefault();
  mScroll->setValue( mStd.updateFrequency() );
  mLCD->display( mStd.updateFrequency() );
  mPopupFullCheck->setChecked( mStd.popupIfFull() );
  mOpenMountCheck->setChecked( mStd.openFileManager() );
  mFileManagerEdit->setText( mStd.fileManager() );

  QListViewItem *item = mList->firstChild();
  if( item != 0 )
  {
    for( int i=mList->header()->count(); i>0; i-- )
    {
        item->setText( i-1, i18n("visible") );
     	item->setPixmap( i-1, UserIcon ( "tick" ) );

    }
  }
}


void KDFConfigWidget::toggleListText( QListViewItem *item, const QPoint &,
				      int column )
{
  if (!item) return;
  QString text = item->text( column );
  item->setText(column, text==i18n("visible")?i18n("hidden"):i18n("visible"));
  item->setPixmap( column, text==i18n("visible") ?  UserIcon ( "delete" )
                                   : UserIcon ( "tick" ) );
}



#include "kdfconfig.moc"
