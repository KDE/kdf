/*
 * kdfwidget.cpp
 *
 * $Id$
 *
 * Copyright (c) 1998-2001 Michael Kropfberger <michael.kropfberger@gmx.net>
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
// 1999-12-05 Espen Sand
// Usage bars should work again.
//

#include <stdlib.h>

#include <qheader.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpainter.h>

#include <kapplication.h>
#include <kcmenumngr.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>

#include "listview.h"
#include "kdfwidget.h"
#include "optiondialog.h"


#define BAR_COLUMN   7
#define FULL_PERCENT 95.0

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif

/**************************************************************/

CListViewItem::CListViewItem( CListView * parent, QListViewItem * after )
  :QListViewItem( parent, after )
{}

// 2001-03-10 Walter F.J. Mueller <W.F.J.Mueller@gsi.de>
// This method returns for all numeric columns the converted number.
// The conversion is done with
//    1. fixed position of the colon
//    2. a sufficient number of leading blanks
// This ensures that the lexical string comparison gives numerical order.

QString CListViewItem::key ( int column, bool ) const
{
  QString tmp;

  switch (column) {
  case KDFWidget::sizeCol:
    tmp.sprintf("%10d",size);
    break;

  case KDFWidget::freeCol:
    tmp.sprintf("%10d",avail);
    break;

  case KDFWidget::fullCol:
  case KDFWidget::usageCol:
    tmp.sprintf("%7.2f",full);
    break;

  default:
    tmp = text(column);
    break;
  }

  return tmp;
}

void CListViewItem::setKeys (int kb_size, int kb_avail, float percent_full)
{
  size  = kb_size;
  avail = kb_avail;
  full  = (percent_full >= 0.) ? percent_full : 0.;
  return;
}

/**************************************************************/

KDFWidget::KDFWidget( QWidget *parent, const char *name, bool init )
  : QWidget(parent, name), mOptionDialog(0), mPopup(0), mTimer(0)
{
  connect(&mDiskList , SIGNAL(readDFDone() ),
           this, SLOT (updateDFDone()) );
  connect(&mDiskList , SIGNAL(criticallyFull(DiskEntry*)),
           this, SLOT (criticallyFull(DiskEntry*)) );

  mTabProp.resize(8);
  mTabProp[0] = new CTabEntry( "Icon", i18n("Icon"), true, 32);
  mTabProp[1] = new CTabEntry( "Device", i18n("Device"), true, 80);
  mTabProp[2] = new CTabEntry( "Type", i18n("Type"), true, 50);
  mTabProp[3] = new CTabEntry( "Size", i18n("Size"), true, 72);
  mTabProp[4] = new CTabEntry( "MountPoint", i18n("Mount Point"), true, 90 );
  mTabProp[5] = new CTabEntry( "Free", i18n("Free"), true, 55);
  // xgettext:no-c-format
  mTabProp[6] = new CTabEntry( "Full%", i18n("Full %"), true, 70);
  mTabProp[7] = new CTabEntry( "UsageBar", i18n("Usage"), true, 100);

  GUI = !init;
  if( GUI )
  {
    QVBoxLayout *topLayout = new QVBoxLayout( this, 0, 0 );
    mList = new CListView( this, "list" );
    topLayout->addWidget( mList );

    mList->setAllColumnsShowFocus( true );
    mList->setFrameStyle( QFrame::WinPanel + QFrame::Sunken );
    mList->setShowSortIndicator(true);
    connect( mList,
      SIGNAL( pressed( QListViewItem *, const QPoint &, int )),
      this, SLOT( rightButtonPressed( QListViewItem *, const QPoint &, int )));
    connect( mList,
      SIGNAL( clicked( QListViewItem *, const QPoint &, int )),
      this, SLOT( rightButtonClicked( QListViewItem *, const QPoint &, int )));
    connect( mList->header(), SIGNAL(sizeChange(int, int, int)),
      this, SLOT(columnSizeChanged(int, int, int)) );
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
}



void KDFWidget::makeColumns( void )
{
  uint i;

  //
  // 1999-11-29 Espen Sand
  // This smells like a bad hack but I need to remove the headers
  // first. If I don't, the list look like shit afterwards. The iterator
  // is just used to prevent an endless loop. With my Qt (1999-11-10 ?)
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
      mList->addColumn( e.mName, e.mWidth );
    else
      mList->addColumn( e.mName, 0 ); // zero width makes it invisible
  }

}




/******************************************************************/
void KDFWidget::closeEvent(QCloseEvent *)
{
  applySettings();
  kapp->quit();
};


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
      if( e.mVisible == true )
      {
	e.mWidth = mList->columnWidth(i);
      }
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
  mStd.updateConfiguration();

  if(GUI)
  {
    KConfig &config = *kapp->config();
    config.setGroup("KDiskFree");
    if( mIsTopLevel == true )  // only "kdf" can be resized
    {
      int w  = config.readNumEntry("Width",width());
      int h = config.readNumEntry("Height",height());
      resize( w, h );
    }

    for( uint i=0; i<mTabProp.size(); i++ )
    {
      CTabEntry &e = *mTabProp[i];
      e.mWidth = config.readNumEntry( e.mRes, e.mWidth );
    }
    if (mTabProp[usageCol]->mWidth > 16)
      mTabProp[usageCol]->mWidth -= 16;

    config.setGroup("KDFConfig");
    for( uint i=0; i<mTabProp.size(); i++ )
    {
      CTabEntry &e = *mTabProp[i];
      e.mVisible = config.readBoolEntry( e.mRes, e.mVisible );
    }

    makeColumns();
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
    if( mOptionDialog == 0 )
    {
      mOptionDialog = new COptionDialog( this, "options", false );
      if( mOptionDialog == 0 ) { return; }
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
  killTimers();
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
  if( mPopup == 0 )
  {
    readingDF = TRUE;
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

  mList->clear();

  int i=0;
  CListViewItem *item = 0;
  for( DiskEntry *disk=mDiskList.first(); disk!=0; disk=mDiskList.next() )
  {
    i++;
    QString size, percent;
    if( disk->kBSize() > 0 )
    {
      percent = KGlobal::locale()->formatNumber(disk->percentFull(), 1) + '%';
      size = disk->prettyKBSize();
    }
    else
    {
      percent = i18n("N/A");
      size    = i18n("N/A");
    }

    int k=0;
    item = new CListViewItem( mList, item );
    bool root = disk->mountOptions().find("user",0,false)==-1 ? true : false;
    item->setPixmap( k++, mList->icon( disk->iconName(), root ) );
    item->setText( k++, disk->deviceName() );
    item->setText( k++, disk->fsType() );
    item->setText( k++, size );
    item->setText( k++, disk->mountPoint() );
    item->setText( k++, disk->prettyKBAvail() );
    item->setText( k++, percent );
    item->setKeys( disk->kBSize(), disk->kBAvail(), disk->percentFull());
  }
  readingDF = false;
  updateDiskBarPixmaps();

  mList->sort();
}

/***************************************************************************
  * Update display
**/
void KDFWidget::resizeEvent( QResizeEvent * )
{
   updateDiskBarPixmaps();
}



/**************************************************************************
  * connected with diskList
**/
void KDFWidget::criticallyFull( DiskEntry *disk )
{
  if( mStd.popupIfFull() == true )
  {
    QString msg = i18n("Device [%1] on [%2] is getting critically full!").
      arg(disk->deviceName()).arg(disk->mountPoint());
    KMessageBox::sorry( this, msg, i18n("Warning"));
  }
}


/**************************************************************************
  * find correct disk related to list item
**/
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

  DiskEntry disk(item->text(deviceCol));
  disk.setMountPoint(item->text(mntCol));

  // I can't get find() to work. The Disks::compareItems(..) is
  // never called.
  //
  //int pos=mDiskList->find(disk);

  int pos = -1;
  for( u_int i=0; i<mDiskList.count(); i++ )
  {
    DiskEntry *item = mDiskList.at(i);
    int res = disk.deviceName().compare( item->deviceName() );
    if( res == 0 )
    {
      res = disk.mountPoint().compare( item->mountPoint() );
    }
    if( res == 0 )
    {
      pos = i;
      break;
    }
  }


  return mDiskList.at(pos);
  //  return(0);
}

void KDFWidget::rightButtonPressed( QListViewItem *item, const QPoint &p, int )
{
  if( KContextMenuManager::showOnButtonPress() == true )
  {
    popupMenu( item, p );
  }
}


void KDFWidget::rightButtonClicked( QListViewItem *item, const QPoint &p, int )
{
  if( KContextMenuManager::showOnButtonPress() == false )
  {
    popupMenu( item, p );
  }
}


/**************************************************************************
  * pops up and asks for mount/umount right-clicked device
**/
void KDFWidget::popupMenu( QListViewItem *item, const QPoint &p )
{
  if (mPopup) //The user may even be able to popup another menu while this open is active...
       return;

  //
  // The list update will be disabled as long as this menu is
  // visible. Reason: The 'disk' may no longer be valid.
  //

  mDiskList.setUpdatesDisabled(true);  
  DiskEntry *disk = selectedDisk( item );
  if( disk == 0 )
  {
    return;
  }

  mPopup = new KPopupMenu( disk->mountPoint(), 0 );
  mPopup->insertItem( i18n("Mount Device"), 0 );
  mPopup->insertItem( i18n("Unmount Device"), 1 );
  mPopup->insertSeparator();
  mPopup->insertItem( i18n("Open in File Manager"), 2 );
  mPopup->setItemEnabled( 0, disk->mounted() ? false : true );
  mPopup->setItemEnabled( 1, disk->mounted() );
  mPopup->setItemEnabled( 2, disk->mounted() );  
  int position = mPopup->exec( p );  
  


  bool openFileManager = false;
  if( position == -1 )
  {
    mDiskList.setUpdatesDisabled(false);
    delete mPopup; mPopup = 0;
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
      else if ( ( mStd.openFileManager() == true)
		&& (position == 0) ) //only on mount
	{
	  openFileManager = true;
	}

      delete item;
      mDiskList.deleteAllMountedAt(disk->mountPoint());
  }
  else if( position == 2 )
  {
    openFileManager = true;
  }

  if( openFileManager == true )
  {
    kdDebug() << "opening filemanager" << endl;
    if(  mStd.fileManager().isEmpty() == false )
    {
      QString cmd = mStd.fileManager();
      int pos = cmd.find("%m");
      if( pos > 0 )
      {
	cmd = cmd.replace( pos, 2, disk->mountPoint() ) + " &";
      }
      else
      {
	cmd += " " + disk->mountPoint() +" &";
      }
      system( QFile::encodeName(cmd) );
    }
  }
  
  //Update only here as showing of error message triggers event loop.
  mDiskList.setUpdatesDisabled(false);
  delete mPopup; mPopup = 0;

  if( position != 2 ) // No need to update when just opening the fm.
  {
    updateDF();
  }
  
}




/**************************************************************************
  * recalculates and repaints the pixBars
**/
void KDFWidget::updateDiskBarPixmaps( void )
{
  if (mTabProp[usageCol]->mVisible != true)
    return;


  int size=0, w=0;

   for(uint i=0; i<mTabProp.size()-1; i++ )
     size += mList->columnWidth(i);
   w=mList->width() - size - 4;
   if (w<0)
     w=0;
   mList->setColumnWidth(usageCol, w );

  int h = mList->fontMetrics().lineSpacing()-2;
  if( h <= 0 )
  {
    return;
  }

  int i=0;
  for(QListViewItem *it=mList->firstChild(); it!=0;it=it->nextSibling(),i++ )
  {
    // I can't get find() to work. The Disks::compareItems(..) is
    // never called.
    //
    //int pos=mDiskList->find(disk);

    DiskEntry dummy(it->text(deviceCol));
    dummy.setMountPoint(it->text(mntCol));
    int pos = -1;
    for( u_int i=0; i<mDiskList.count(); i++ )
      {
	DiskEntry *item = mDiskList.at(i);
	int res = dummy.deviceName().compare( item->deviceName() );
	if( res == 0 )
	  {
	    res = dummy.mountPoint().compare( item->mountPoint() );
	  }
	if( res == 0 )
	  {
	    pos = i;
	    break;
	  }
      }


     DiskEntry *disk = mDiskList.at(pos);
    if( disk == 0 ) { continue; }

    if( disk->mounted() == true && disk->percentFull() != -1 )
    {
      int w = mList->columnWidth(usageCol)-2;
      if( w <= 0 ) { continue; }

      QPixmap *pix = new QPixmap( w, h );
      if( pix == 0 ) { continue; }

      pix->fill(white);
      QPainter p(pix);
      p.setPen(black);
      p.drawRect(0,0,w,h);
      QColor c;
      if ( (disk->iconName().find("cdrom") != -1)
	   || (disk->iconName().find("writer") != -1) )
	c = gray;
      else
	c = disk->percentFull() > FULL_PERCENT ? red : darkGreen;
      p.setBrush(c );
      p.setPen(white);
      p.drawRect(1,1,(int)(((float)pix->width()-2)*(disk->percentFull()/100)),
		 pix->height()-2);
      it->setPixmap ( usageCol, *pix );
      p.end();
      delete pix;
    }
  }
}


void KDFWidget::columnSizeChanged( int, int, int )
{

  if( mTimer == 0 )
  {
    mTimer = new QTimer( this );
    connect( mTimer, SIGNAL(timeout()), this, SLOT(updateDiskBarPixmaps()) );
  }
  else if( mTimer->isActive() == true )
  {
    mTimer->stop();
  }

  mTimer->start( 10, true );
}


void KDFWidget::invokeHelp()
{
  kapp->invokeHelp("", "kcontrol/kdf");
}

#include "kdfwidget.moc"

