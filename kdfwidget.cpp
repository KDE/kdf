/*
 * kdfwidget.cpp
 *
 * $Id$
 *
 * Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
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

#include <stdlib.h>

#include <qheader.h>
#include <qstring.h> 
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpaintdevice.h>

#include <kapp.h> 
#include <kcontrol.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kpopmenu.h>

#include "listview.h"
#include "kdfwidget.h"
#include "optiondialog.h"


#define PIX_COLUMN 7
#define FULL_PERCENT 95.0

#define DEFAULT_FREQ 60
#define DEFAULT_FILEMGR_COMMAND "kfmclient openURL %m"

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif


KDFWidget::KDFWidget( QWidget *parent, const char *name, bool init )
  : KConfigWidget(parent, name), mOptionDialog(0), mPopup(0)
{
  mPopupIfFull = true;
  openFileMgrOnMount=FALSE;
  updateFreq=DEFAULT_FREQ;
  connect(&diskList , SIGNAL(readDFDone() ),
           this, SLOT (updateDFDone()) );
  connect(&diskList , SIGNAL(criticallyFull(DiskEntry*)),
           this, SLOT (criticallyFull(DiskEntry*)) );

  mTabProp.resize(8);
  mTabProp[0] = new CTabEntry( "Icon", i18n("Icon"), true, 32);
  mTabProp[1] = new CTabEntry( "Device", i18n("Device"), false, 80);
  mTabProp[2] = new CTabEntry( "Type", i18n("Type"), true, 50);
  mTabProp[3] = new CTabEntry( "Size", i18n("Size"), true, 72);
  mTabProp[4] = new CTabEntry( "MountPoint", i18n("Mount point"), true, 90 );
  mTabProp[5] = new CTabEntry( "Free", i18n("Free"), true, 55);
  mTabProp[6] = new CTabEntry( "Full%", i18n("Full %"), true, 70);
  mTabProp[7] = new CTabEntry( "UsageBar", i18n("Usage"), true, 100);

  GUI = init == true ? FALSE : TRUE;
  if( GUI )
  {
    QVBoxLayout *topLayout = new QVBoxLayout( this, 0, 0 );
    mList = new CListView( this, "list" );
    topLayout->addWidget( mList );

    mList->setAllColumnsShowFocus( true );
    mList->setFrameStyle( QFrame::WinPanel + QFrame::Sunken );
    mList->setShowSortIndicator(true);
    connect( mList, 
      SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int )),
      this, SLOT( popupMenu( QListViewItem *, const QPoint &, int )));
    makeColumns();

    mIsTopLevel = QString(parent->className()) == "KDFTopLevel" ? true : false;
  }
  
  loadSettings();
  if( init == true )
  {
    applySettings();
  }
}


KDFWidget::~KDFWidget() 
{
}; 



void KDFWidget::makeColumns( void )
{
  uint i;

  //
  // 1999-11-29 Espen Sand
  // This smells like a bad hack but I need to to remove the headers
  // first. If I don't, the list look like shit afterwards. The iterator
  // is just used to pervent an endless loop. With my Qt (1999-11-10 ?)
  // I only need as many iterations as there are header items but who knows
  // what a new Qt can do!
  //
  for( i=1000; mList->header()->count() > 0 && i>0; i-- )
  {
    mList->header()->removeLabel( 0 );
    mList->header()->update();
  }
  for( i=0; i<mTabProp.size(); mList->removeColumn(i++) );
  mList->clear();

  for( i=0; i<mTabProp.size(); i++ )
  {
    CTabEntry &e = *mTabProp[i];
    if( e.mVisible == true )
    {
      mList->addColumn( e.mName, e.mWidth );
    }
  }

}





void KDFWidget::closeEvent(QCloseEvent *)
{
  applySettings(); 
  kapp->quit();
};


void KDFWidget::applySettings( void )
{
  KConfig &config = *kapp->config();

  config.setGroup("KDiskFree");
  if( mIsTopLevel == true ) 
  {
    config.writeEntry( "Width", width() );
    config.writeEntry( "Height", height() );
  }
  if( GUI ) 
  {
    for( uint i=0; i<mTabProp.size(); i++ )
    {
      CTabEntry &e = *mTabProp[i];
      config.writeEntry( e.mRes, e.mWidth );
    }
  }
  config.sync();
  updateDF();
}


/***************************************************************************
  * reads the KConfig
**/
void KDFWidget::loadSettings( void )
{
  KConfig &config = *kapp->config();

  config.setGroup("KDFConfig");
  updateFreq = config.readNumEntry("UpdateFrequency", DEFAULT_FREQ );
  fileMgr = config.readEntry("FileManagerCommand", DEFAULT_FILEMGR_COMMAND );
  mPopupIfFull = config.readBoolEntry("PopupIfFull", mPopupIfFull );
  openFileMgrOnMount = config.readBoolEntry("OpenFileMgrOnMount",
					    openFileMgrOnMount );

  if(GUI) 
  {
    if( mIsTopLevel == true )  // only "kdf" can be resized
    {
      config.setGroup("KDiskFree");
      int w  = config.readNumEntry("Width",width());
      int h = config.readNumEntry("Height",height());
      resize( w, h );
    }

    for( uint i=0; i<mTabProp.size(); i++ )
    {
      CTabEntry &e = *mTabProp[i];
      e.mWidth = config.readNumEntry( e.mRes, e.mWidth );
    }

    config.setGroup("KDFConfig");
    for( uint i=0; i<mTabProp.size(); i++ )
    {
      CTabEntry &e = *mTabProp[i];
      e.mVisible = config.readBoolEntry( e.mRes, e.mVisible );
    }

    makeColumns();
    setUpdateFreq(updateFreq);
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
    if( mOptionDialog == 0 )
    {
      mOptionDialog = new COptionDialog( this, "options", false );
      if( mOptionDialog == 0 ) { return; }
      connect( mOptionDialog, SIGNAL(valueChanged()), SLOT(loadSettings()) );
    }
    mOptionDialog->show();
  }
}


/***************************************************************************
  * resets the timer for automatic df-refreshes
**/
void KDFWidget::setUpdateFreq( int freq )
{
  killTimers(); //kills !all! running timers
  updateFreq = freq;
  if( updateFreq > 0 )
  {
    // 0 sets to NO_AUTO_UPDATE   ;)
    startTimer( updateFreq * 1000 );
  }
  applySettings();
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
  if( mPopup == 0 )
  {
    readingDF = TRUE;
    diskList.readFSTAB();
    diskList.readDF(); 
  }
};

/***************************************************************************
  * gets the signal when the diskList is complete and up to date
**/
void KDFWidget::updateDFDone( void )
{
  mList->clear();

  int i=0;
  QListViewItem *item = 0;
  for( DiskEntry *disk=diskList.first(); disk!=0; disk=diskList.next() ) 
  {
    i++;
    QString size, percent;
    if( disk->kBSize() > 0 ) 
    {
      percent.sprintf("%2.1f%%",disk->percentFull() );
      size = disk->prettyKBSize();
    }
    else
    {
      percent = i18n("Na");
      size    = i18n("Na");
    }

    int k=0;
    item = new QListViewItem( mList, item );
    if( mTabProp[0]->mVisible == true )
    {
      bool root = disk->mountOptions().find("user",0,false)==-1 ? true : false;
      item->setPixmap( k++, mList->icon( disk->iconName(), root ) );
    }
    if( mTabProp[1]->mVisible == true )
      item->setText( k++, disk->deviceName() );
    if( mTabProp[2]->mVisible == true )
      item->setText( k++, disk->fsType() );
    if( mTabProp[3]->mVisible == true )
      item->setText( k++, size );
    if( mTabProp[4]->mVisible == true )
      item->setText( k++, disk->mountPoint() );
    if( mTabProp[5]->mVisible == true )
      item->setText( k++, disk->prettyKBAvail() );
    if( mTabProp[6]->mVisible == true )
      item->setText( k++, percent );
  }
  readingDF = false;
};



 
/**************************************************************************
  * connected with diskList
**/
void KDFWidget::criticallyFull( DiskEntry *disk )
{
  if( mPopupIfFull == true )
  {
    QString msg = i18n("Device [%1] on [%1] is getting critically full!").
      arg(disk->deviceName()).arg(disk->mountPoint());
    KMessageBox::sorry( this, msg, i18n("Warning"));
  }
}


DiskEntry *KDFWidget::selectedDisk( QListViewItem *item )
{
  if( item == 0 )
  {
    item = mList->selectedItem();
  }
  if( item == 0 )
  {
    return(0);
  }

  int i=0;
  for(QListViewItem *it=mList->firstChild(); it!=0;it=it->nextSibling(),i++ )
  {
    if( it == item )
    {
      return( diskList.at(i) );
    }
  }

  return(0);
}
  

/**************************************************************************
  * pops up and asks for mount/umount right-clicked device
**/
void KDFWidget::popupMenu( QListViewItem *item, const QPoint &p, int )
{
  DiskEntry *disk = selectedDisk( item );
  if( disk == 0 )
  {
    return;
  }

  //
  // The list update will be disabled as long as this menu is 
  // visible. Reason: The 'disk' may no longer be valid. 
  //
  mPopup = new KPopupMenu( disk->mountPoint(), 0 );
  mPopup->insertItem( i18n("Mount device"), 0 );
  mPopup->insertItem( i18n("Unmount device"), 1 );
  mPopup->insertSeparator();
  mPopup->insertItem( i18n("Open filemanager"), 2 );
  mPopup->setItemEnabled( 0, disk->mounted() ? false : true );
  mPopup->setItemEnabled( 1, disk->mounted() );
  mPopup->setItemEnabled( 2, disk->mounted() );
  int position = mPopup->exec( p );
  delete mPopup; mPopup = 0;


  bool openFileManager = false;
  if( position == -1 )
  {
    return;
  }
  else if( position == 0 || position == 1 )
  {
    item->setText( sizeCol, i18n("MOUNTING") );
    item->setText( freeCol, i18n("MOUNTING") );
    item->setPixmap( 0, mList->icon( "mini-clock", false ) );

    int val = disk->toggleMount();
    if( val != 0 /*== false*/ )
    {
      KMessageBox::error( this, disk->lastSysError() );
    }
    else if( openFileMgrOnMount == false || disk->mounted() == true )
    {
      openFileManager = true;
    }
  }
  else if( position == 2 )
  {
    openFileManager = true;
  }

  if( openFileManager == true )
  {
    if( fileMgr.isEmpty() == false ) 
    {
      QString cmd = fileMgr;
      int pos = cmd.find("%m");
      if( pos > 0 )
      {
	cmd = cmd.replace( pos, 2, disk->mountPoint() ) + " &";
      }
      else
      {
	cmd += " " + disk->mountPoint() +" &";
      }
      system( cmd.latin1() );
    }
  }

  if( position != 2 ) // No need to update when just opening the fm.
  {
    updateDF(); 
  }
}




/**************************************************************************
  * recalculates and repaints the pixBars
**/
void KDFWidget::updatePixmaps( void )
{
  #if 0
  debug("KDFWidget::updatePixmaps");
  tabList->setAutoUpdate(FALSE);
  tabList->unmarkAll();
  for ( DiskEntry *disk=diskList.first(); disk != 0; disk=diskList.next() ) {
    if ( !disk->mounted() ) { 
       unsigned int ipos=0;
       QString e1, e2;
       while(ipos<tabList->count()) {  //find old entry in tabList
          e1=tabList->text(ipos,1);
	  e2=tabList->text(ipos,4);
	  if ((e1==(const char *)disk->deviceName())
	    && (e2==(const char *)disk->mountPoint())) {
              //entry found
	    //tabList->markItem(ipos);
             tabList->changeItemColor(lightGray,ipos);
             QString fullbar;
             fullbar.sprintf("%s%s%s","BAR"
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
             tabList->dict().remove((const char *)fullbar);
             tabList->changeItemPart("",ipos,PIX_COLUMN);
          }//if
        ipos++;
        }//while        
    }//if not mounted
    else if (disk->percentFull() != -1) { //always true for a mounted disk ?!
      if (disk->percentFull() > FULL_PERCENT) {
       unsigned int ipos=0;
       QString e1, e2;
       while(ipos<tabList->count()) {  //find old entry in tabList
          e1=tabList->text(ipos,1);
	  e2=tabList->text(ipos,4);
	  if ((e1==(const char *)disk->deviceName()) 
	    && (e2==(const char *)disk->mountPoint())) {
                                    //entry found
	    //tabList->markItem(ipos);
             tabList->changeItemColor(red,ipos);
          }//if
        ipos++;
        }//while        
      }//if full over FULL_PERCENT
      
      if (tabList->cellWidth(PIX_COLUMN) > 7) {
        QPixmap *pix;
        pix = new QPixmap(tabList->cellWidth(PIX_COLUMN)-7
                      , tabList->cellHeight(1)-1 );
        CHECK_PTR(pix);
        pix->fill(white );
        QPainter p(pix);
        p.setPen(black);
        p.drawRect(0,0,pix->width(),pix->height());
        if (disk->percentFull() > FULL_PERCENT)
            p.setBrush(red);
        else
            p.setBrush(darkBlue);
        p.setPen(white);
        p.drawRect(1,1
             ,(int)(  ((float)pix->width()-2) 
                     * (disk->percentFull()/100) )
             ,pix->height()-2 );
       QString fullbar;
       fullbar.sprintf("%s%s%s","BAR"
                            ,disk->deviceName().latin1()
                            ,disk->mountPoint().latin1());
        tabList->dict().replace((const char *)fullbar, pix);
      }//if usageBar is visible
    }//was mounted
  }//for
  tabList->setAutoUpdate(TRUE);
  tabList->repaint();
  #endif
}


/**************************************************************************
  * calculates the sizes and shows all devices
**/
void KDFWidget::paintEvent(QPaintEvent *)
{
  #if 0
  debug("KDFWidget::paintEvent");

  //resizing of PIX_COLUMN *************************************
  if (tabList->columnWidth(PIX_COLUMN) != 0 ){
    int totalWidth=0;
    for (int i=0;i<PIX_COLUMN;i++) //all except the last (pix) column
      totalWidth += tabList->columnWidth(i);
    //adjust size to fit completely into tabList-widget...
    if (tabList->width() > totalWidth-4)
       tabList->setColumnWidth(PIX_COLUMN,tabList->width()-totalWidth-4);
  }//if user hasn't turned of the usageBar

  updatePixmaps();
  #endif
}


void KDFWidget::invokeHTMLHelp()
{
  kapp->invokeHTMLHelp("kcontrol/kdf/index.html","");
}

#include "kdfwidget.moc"

