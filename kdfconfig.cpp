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
#include <qfile.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qscrollbar.h>
#include <qstring.h>

#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>

#include "listview.h"
#include "kdfconfig.h"

#define DEFAULT_FREQ 60
#define DEFAULT_FILEMGR_COMMAND "kfmclient openURL %m"

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif


KDFConfigWidget::KDFConfigWidget(QWidget *parent, const char *name, bool init)
  : KConfigWidget (parent, name)
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
    }
    mList->setSelected( mListItem, true );

    QGridLayout *gl = new QGridLayout( 2, 2 );
    topLayout->addLayout( gl );
    gl->setColStretch( 1, 10 );  

    mScroll = new QScrollBar( this ); 
    CHECK_PTR(mScroll); 
    mScroll->setOrientation( QScrollBar::Horizontal );
    mScroll->setSteps(1,20);
    mScroll->setRange(0, 180 ); 
    mScroll->setValue(DEFAULT_FREQ);
    gl->addWidget( mScroll, 1, 1 );  

    mLCD = new QLCDNumber( this ); 
    CHECK_PTR(mLCD);
    mLCD->setNumDigits( 3 );
    mLCD->setSegmentStyle(QLCDNumber::Filled);
    connect(mScroll,SIGNAL(valueChanged(int)),mLCD,SLOT(display(int)));
    gl->addMultiCellWidget( mLCD, 0, 1, 0, 0 );

    QLabel *label = new QLabel( i18n("Update frequency (seconds)"), this ); 
    CHECK_PTR(label);
    gl->addWidget( label, 0, 1 );  


    label = new QLabel( i18n("FileManager (e.g. kvt -e mc %m)") ,this);
    CHECK_PTR(label);
    topLayout->addWidget( label );

    mFileManagerEdit = new QLineEdit( this ); 
    CHECK_PTR(mFileManagerEdit);
    topLayout->addWidget( mFileManagerEdit );

    text = i18n("Open the above filemanager on mount");
    mOpenMountCheck = new QCheckBox(text, this ); 
    CHECK_PTR(mOpenMountCheck);
    topLayout->addWidget( mOpenMountCheck );

    text = i18n("Pop up a window when a disk gets critically full");
    mPopupFullCheck = new QCheckBox( text, this ); 
    CHECK_PTR(mPopupFullCheck);
    topLayout->addWidget( mPopupFullCheck );

    text = parent->className();
    isTopLevel = text == "KDFTopLevel" ? TRUE : FALSE;
  }

  loadSettings();
  if( init )
  {
    applySettings();
  }
}


KDFConfigWidget::~KDFConfigWidget() 
{ 
}; 


void KDFConfigWidget::closeEvent(QCloseEvent *)
{
  debug("KDFConfigWidget::closeEvent");
  applySettings(); 
  kapp->quit();
};


void KDFConfigWidget::applySettings( void )
{
  KConfig &config = *kapp->config();
  config.setGroup("KDFConfig");

  if( GUI ) 
  {
    config.writeEntry( "Width", width() );
    config.writeEntry( "Height", height() );
    config.writeEntry( "UpdateFrequency", mScroll->value() );
    config.writeEntry( "FileManagerCommand", mFileManagerEdit->text() );
    config.writeEntry( "PopupIfFull", mPopupFullCheck->isChecked() );
    config.writeEntry( "OpenFileMgrOnMount", mOpenMountCheck->isChecked() );

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
    config.writeEntry( "FileManagerCommand", DEFAULT_FILEMGR_COMMAND );
  }
  config.sync();
}


void KDFConfigWidget::loadSettings( void )
{
  KConfig &config = *kapp->config();
  config.setGroup("KDFConfig");

  if (GUI) 
  {
    if( isTopLevel ) 
    {
      //int appWidth=config->readNumEntry("Width",width());
      //int appHeight=config->readNumEntry("Height",height());
      //resize(appWidth,appHeight);
    }
    mScroll->setValue(config.readNumEntry("UpdateFrequency",DEFAULT_FREQ));
    mPopupFullCheck->setChecked(config.readBoolEntry("PopupIfFull",TRUE));
    mOpenMountCheck->setChecked(config.readBoolEntry("OpenFileMgrOnMount",
						     FALSE));
    mLCD->display(mScroll->value());
    mFileManagerEdit->setText( config.readEntry( "FileManagerCommand",
						 DEFAULT_FILEMGR_COMMAND));

    QListViewItem *item = mList->firstChild();
    if( item != 0 )
    {
      for( int i=mList->header()->count(); i>0; i-- )
      {
	int j = config.readNumEntry( mTabName[i-1]->mRes, 1 );
	item->setText( i-1, j==0 ? i18n("hidden") : i18n("visible") );
      }
    }
  }

}

void KDFConfigWidget::defaultsBtnClicked( void )
{
  mScroll->setValue( DEFAULT_FREQ );
  mFileManagerEdit->setText( DEFAULT_FILEMGR_COMMAND );
  mPopupFullCheck->setChecked( true );
  mOpenMountCheck->setChecked( false );

  QListViewItem *item = mList->firstChild();
  if( item != 0 )
  {
    for( int i=mList->header()->count(); i>0; i-- )
    {
      item->setText( i-1, i18n("visible") );
    }
  }
}


void KDFConfigWidget::toggleListText( QListViewItem *item, const QPoint &,
				      int column )
{
  QString text = item->text( column );
  item->setText(column, text==i18n("visible")?i18n("hidden"):i18n("visible"));
}



#include "kdfconfig.moc"






