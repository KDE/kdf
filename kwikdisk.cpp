/*
  kwikdisk.cpp - KDiskFree

  $Id$

  written 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  */

//
// 1999-12-03 Espen Sand
// Cleanups, improvements and fixes for KDE-2
//


#include <qbitmap.h>
#include <qpainter.h>
#include <qcursor.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <kwin.h>
#include <kcmdlineargs.h>

#include <stdlib.h>

#include "kwikdisk.h"
#include "optiondialog.h"

static const char *description =
	I18N_NOOP("KDE Free disk space utility (another one?)");

static const char *version = "v0.0.1";


/***************************************************************/
/***************************************************************/
/***************************************************************/
MyToolTip::MyToolTip( QWidget *parent, QToolTipGroup *group )
  : QToolTip(parent,group)
{
  mEnableTipping = false;
}

MyToolTip::~MyToolTip( void )
{
}


void MyToolTip::setTipping( bool enableTipping )
{
  mEnableTipping = enableTipping;
}


void MyToolTip::setPossibleTip( const QRect &rect, const QString &text )
{
  mRect = rect;
  mText = text;
  setTipping(true);
}


void MyToolTip::maybeTip(const QPoint &)
{
  if( mEnableTipping == true && mText.isNull() == false )
  {
    tip( mRect, mText );
  }
}

/***************************************************************/
/***************************************************************/
/***************************************************************/
MyPopupMenu::MyPopupMenu(QWidget *parent, const char *name)
  : QPopupMenu(parent,name), mCurrentId(-1), mCurrentIndex(-1)
{
  mToolTip = new MyToolTip(this);
  mToolTipStrings.setAutoDelete(true);
  connect(this,SIGNAL(aboutToHide()),this,SLOT(aboutToHide()));
  connect(this,SIGNAL(aboutToShow()),this,SLOT(aboutToShow()));
}


MyPopupMenu::~MyPopupMenu( void )
{
  delete mToolTip;
}


QRect MyPopupMenu::itemRectangle( int id )
{
  int x = contentsRect().x();
  int y = contentsRect().y();
  int w = contentsRect().width();

  for( QMenuItemListIt it( *mitems ); it.current() != 0; ++it )
  {
    if( it.current()->id() == id )
    {
      return( QRect( x, y, w, itemHeight(it.current())) );
    }
    y += itemHeight(it.current());
  }

  return( QRect(0,0,0,0) );
}


void MyPopupMenu::setToolTip( int id, const QString & text )
{
  if( !text.isNull() )
  {
    mToolTipStrings.replace( id, new QString(text) );
  }
}


void MyPopupMenu::aboutToHide()
{
  disconnect(this,SIGNAL(highlighted(int)),this,SLOT(registerActiveItem(int)));
}

void MyPopupMenu::aboutToShow()
{
  connect(this,SIGNAL(highlighted(int)),this,SLOT(registerActiveItem(int)));
}

void MyPopupMenu::registerActiveItem( int id )
{
  mCurrentIndex = indexOf(id);

  if( id != -1 && id != mCurrentId )
  {
    QRect r = itemRectangle(id);
    if( r.height() > 0 )
    {
      QString *str = mToolTipStrings[id];
      if( str != 0 )
      {
	mToolTip->setPossibleTip( r, *str );
	mCurrentId = id;
      }
    }
  }
}


int MyPopupMenu::activeIndex( void )
{
  return( mCurrentIndex );
}





/***************************************************************/
/***************************************************************/
/***************************************************************/
DockWidget::DockWidget(QWidget *parent, const char *name)
  : QLabel(parent,name), mPopupMenu(0), mOptionDialog(0), mReadingDF(false),
    mDirty(true)
{
  connect( &mDiskList, SIGNAL(readDFDone()), this, SLOT(updateDFDone()) );
  connect( &mDiskList , SIGNAL(criticallyFull(DiskEntry*)),
           this, SLOT(criticallyFull(DiskEntry*)) );
  loadSettings();
}


DockWidget::~DockWidget( void )
{
  delete mPopupMenu;
}



/***************************************************************************
  * reads the KConfig
**/
void DockWidget::loadSettings( void )
{
  mStd.updateConfiguration();
  setUpdateFrequency( mStd.updateFrequency() );
}


/**************************************************************************
  * connected with diskList
**/
void DockWidget::criticallyFull( DiskEntry *disk )
{
  if( mStd.popupIfFull() == true )
  {
    QString msg = i18n("Device [%1] on [%2] is getting critically full!").
      arg(disk->deviceName()).arg(disk->mountPoint());
    KMessageBox::sorry( this, msg, i18n("Warning"));
  }
}



DiskEntry *DockWidget::selectedDisk( void )
{
  if( mPopupMenu == 0 || mPopupMenu->activeIndex() == -1 )
  {
    return( 0 );
  }

  kdDebug() << "selectedDisk: "
	    << mDiskList.at(mPopupMenu->activeIndex())->mountPoint()
	    << "==> index: "
	    << mPopupMenu->activeIndex() << endl;

  return( mDiskList.at(mPopupMenu->activeIndex()) );
}



/***************************************************************************
  * resets the timer for automatic df-refreshes
**/
void DockWidget::setUpdateFrequency(int frequency )
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
 * Mark the list as dirty thus forcing a reload the next time the
 * popup menu is about to become visible. Note: A current visible popup
 * will not be update now.
 **/
void DockWidget::timerEvent(QTimerEvent *)
{
  mDirty = true;
}


void DockWidget::startKDF( void )
{
  system("kdf &");
}


void DockWidget::mousePressEvent( QMouseEvent * )
{
  if( mPopupMenu != 0 && mDirty == false )
  {
    showPopupMenu();
  }
  else
  {
    updateDF();
  }
}


void DockWidget::sysCallError( DiskEntry *disk, int err_no )
{
  if( err_no != 0 )
  {
    KMessageBox::sorry( this, disk->lastSysError() );
  }
}


/***************************************************************************
  * checks fstab & df
**/
void DockWidget::updateDF( void )
{
  mReadingDF = true;
  mDiskList.readFSTAB();
  mDiskList.readDF();
}


void DockWidget::toggleMount( void )
{
  kdDebug() << "toggleMount" << endl;

  DiskEntry *disk = selectedDisk();
  if( disk == 0 )
  {
    return;
  }

  int val = disk->toggleMount();
  if( val != 0 )
  {
    KMessageBox::error( this, disk->lastSysError() );
  }
  else if( (mStd.openFileManager() == true) && (disk->mounted() == true ))
  {
    kdDebug() << "opening filemanager" << endl;
    if( mStd.fileManager().isEmpty() == false )
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
  mDirty = true;
}


void DockWidget::updateDFDone( void )
{
  mReadingDF = false;
  mDirty     = false;

  if (mPopupMenu!=0) delete mPopupMenu;
  mPopupMenu = new MyPopupMenu; Q_CHECK_PTR(mPopupMenu);

  for( DiskEntry *disk = mDiskList.first(); disk!=0; disk = mDiskList.next())
  {
    QString toolTipText = i18n("%1 (%2) %3 on %4")
      .arg( disk->mounted() ? i18n("Unmount") : i18n("Mount"))
      .arg(disk->fsType()).arg(disk->deviceName()).arg(disk->mountPoint());

    QString entryName = disk->mountPoint();
    if( disk->mounted() )
    {
      entryName += QString("   [%1]").arg(disk->prettyKBAvail());
    }
    int id = mPopupMenu->insertItem("",this, SLOT(toggleMount()) );

    QPixmap *pix = new QPixmap(SmallIcon(disk->iconName()));

    if( getuid() !=0 && disk->mountOptions().find("user",0, false) == -1 )
    {
      //
      // Special root icon, normal user can´t mount.
      //
      // 2000-01-23 Espen Sand
      // Careful here: If the mask has not been defined we can
      // not use QPixmap::mask() because it returns 0 => segfault
      //
      if( pix->mask() != 0 )
      {
	QBitmap *bm = new QBitmap(*(pix->mask()));
	if( bm != 0 )
	{
	  QPainter qp( bm );
	  qp.setPen(QPen(white,1));
	  qp.drawRect(0,0,bm->width(),bm->height());
	  qp.end();
	  pix->setMask(*bm);
	}
	QPainter qp( pix );
	qp.setPen(QPen(red,1));
	qp.drawRect(0,0,pix->width(),pix->height());
	qp.end();
      }
      mPopupMenu->disconnectItem(id,disk,SLOT(toggleMount()));
      toolTipText = i18n("You must login as root to mount this disk");
    }

    mPopupMenu->changeItem(*pix,entryName,id);
    // mPopupMenu->changeItem(DevIcon(disk->iconName()),entryName,id);
    //    connect(disk, SIGNAL(sysCallError(DiskEntry *, int) ),
    //      this, SLOT(sysCallError(DiskEntry *, int)) );
    mPopupMenu->setToolTip(id, toolTipText );
  }

  mPopupMenu->insertSeparator();

  mPopupMenu->insertItem(
    SmallIcon( "kdf" ),
    i18n("&Start KDiskFree"), this, SLOT(startKDF()),0);

  mPopupMenu->insertItem(
    BarIcon( "configure" ),
    i18n("&Customize"), this, SLOT(settingsBtnClicked()),0);

  mPopupMenu->insertItem(
    BarIcon( "help" ),
    i18n("&Help"), this, SLOT(invokeHelp()),0);

  mPopupMenu->insertSeparator();

  mPopupMenu->insertItem(
    BarIcon( "exit" ),
    i18n("&Quit"), this, SLOT(quit()) );

  mPopupMenu->move(-1000,-1000);
  showPopupMenu();

  QWidget::setCursor(ArrowCursor);
}


void DockWidget::showPopupMenu( void )
{
  if( mPopupMenu == 0 )
  {
    return;
  }

  QRect g = KWin::info(winId()).geometry;
  QSize s = mPopupMenu->sizeHint();
  int scnum = QApplication::desktop()->screenNumber(this);
  QRect desk = QApplication::desktop()->screenGeometry(scnum);

  if( g.x() > desk.center().x() &&
      g.y() + s.height() > desk.bottom() )
  {
    mPopupMenu->popup(QPoint( g.x(), g.y() - s.height()));
  }
  else
  {
    mPopupMenu->popup(QPoint( g.x() + g.width(), g.y() + g.height()));
  }
}





/***************************************************************************
  * pops up the SettingsBox if the settingsBtn is clicked
**/
void DockWidget::settingsBtnClicked( void )
{
  if( mOptionDialog == 0 )
  {
    mOptionDialog = new COptionDialog( this, "options", false );
    if( mOptionDialog == 0 ) { return; }
    connect( mOptionDialog, SIGNAL(valueChanged()),
	     this, SLOT(loadSettings()) );
  }
  mOptionDialog->show();
}


void DockWidget::invokeHelp( void )
{
  kapp->invokeHelp("", "kcontrol/kdf" );
}


void DockWidget::quit( void )
{
  emit quitProgram();
}


/***************************************************************/
KwikDiskTopLevel::KwikDiskTopLevel(QWidget *, const char *name)
  : KMainWindow(0, name)
{
  setPlainCaption("kwikdisk");

  mDockIcon = new DockWidget(this,"docked_icon");
  connect( mDockIcon, SIGNAL(quitProgram()), this, SLOT(close()) );
  mDockIcon->setPixmap(SmallIcon("kdf"));
  setCentralWidget( mDockIcon );
  resize(24,24);
}


KwikDiskTopLevel::~KwikDiskTopLevel( void )
{
  delete mDockIcon;
}


bool KwikDiskTopLevel::queryExit( void )
{
  return( true );
}




/***************************************************************/

int main(int argc, char **argv)
{
  KCmdLineArgs::init(argc, argv, "kdf", description, version);

  KApplication app;

  KwikDiskTopLevel *ktl = new KwikDiskTopLevel();
  Q_CHECK_PTR(ktl);
  app.setMainWidget(ktl);
  KWin::setSystemTrayWindowFor(ktl->winId(), 0);
  ktl->show();

  return app.exec();
};


#include "kwikdisk.moc"

