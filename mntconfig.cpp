/*
 * mntconfig.cpp
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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
//

#include <qgroupbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlineedit.h>

#undef Unsorted

#include <kapplication.h>
#include <kfiledialog.h>
#include <kicondialog.h>
#include <kmessagebox.h>

#include "listview.h"
#include "mntconfig.h"

#ifndef KDE_USE_FINAL
static bool GUI;
#endif


MntConfigWidget::MntConfigWidget(QWidget *parent, const char *name, bool init)
  : QWidget(parent, name)
{
  mInitializing = false;

  GUI = !init;
  if (GUI)
  {
    //tabList fillup waits until disklist.readDF() is done...
    mDiskList.readFSTAB();
    mDiskList.readDF();
    mInitializing = true;
    connect( &mDiskList,SIGNAL(readDFDone()),this,SLOT(readDFDone()));

    QString text;
    QVBoxLayout *topLayout = new QVBoxLayout( this, 0, KDialog::spacingHint());

    mList = new CListView( this, "list", 8 );
    mList->setAllColumnsShowFocus( true );
    mList->addColumn( i18n("Icon") );
    mList->addColumn( i18n("Device") );
    mList->addColumn( i18n("Mount Point") );
    mList->addColumn( i18n("Mount Command") );
    mList->addColumn( i18n("Unmount Command") );
    mList->setFrameStyle( QFrame::WinPanel + QFrame::Sunken );
    connect( mList, SIGNAL(selectionChanged(QListViewItem *)),
	     this, SLOT(clicked(QListViewItem *)));

    topLayout->addWidget( mList );

    text = QString("%1: %2  %3: %4").
      arg(mList->header()->label(DEVCOL)).
      arg(i18n("None")).
      arg(mList->header()->label(MNTPNTCOL)).
      arg(i18n("None"));
    mGroupBox = new QGroupBox( text, this );
    Q_CHECK_PTR(mGroupBox);
    topLayout->addWidget(mGroupBox);

    QGridLayout *gl = new QGridLayout(mGroupBox, 3, 4, KDialog::spacingHint());
    if( gl == 0 ) { return; }
    gl->addRowSpacing( 0, fontMetrics().lineSpacing() );

    mIconLineEdit = new QLineEdit(mGroupBox);
    Q_CHECK_PTR(mIconLineEdit);
    mIconLineEdit->setMinimumWidth( fontMetrics().maxWidth()*10 );
    connect( mIconLineEdit, SIGNAL(textChanged(const QString&)),
	     this,SLOT(iconChanged(const QString&)));
    connect( mIconLineEdit, SIGNAL(textChanged(const QString&)),
	     this,SLOT(slotChanged()));
    gl->addWidget( mIconLineEdit, 2, 0 );

    mIconButton = new KIconButton(mGroupBox);
    mIconButton->setIconType(KIcon::Small, KIcon::Device);
    Q_CHECK_PTR(mIconButton);
    mIconButton->setFixedWidth( mIconButton->sizeHint().height() );
    connect(mIconButton,SIGNAL(iconChanged(QString)),this,SLOT(iconChangedButton(QString)));
    gl->addWidget( mIconButton, 2, 1 );

    //Mount
    mMountButton = new QPushButton( i18n("Get Mount Command"), mGroupBox );
    Q_CHECK_PTR(mMountButton);
    connect(mMountButton,SIGNAL(clicked()),this,SLOT(selectMntFile()));
    gl->addWidget( mMountButton, 1, 2 );

    mMountLineEdit = new QLineEdit(mGroupBox);
    Q_CHECK_PTR(mMountLineEdit);
    mMountLineEdit->setMinimumWidth( fontMetrics().maxWidth()*10 );
    connect(mMountLineEdit,SIGNAL(textChanged(const QString&)),
	    this,SLOT(mntCmdChanged(const QString&)));
    connect( mMountLineEdit, SIGNAL(textChanged(const QString&)),
	     this,SLOT(slotChanged()));
    gl->addWidget( mMountLineEdit, 1, 3 );

    //Umount
    mUmountButton = new QPushButton(i18n("Get Unmount Command"), mGroupBox );
    Q_CHECK_PTR( mUmountButton );
    connect(mUmountButton,SIGNAL(clicked()),this,SLOT(selectUmntFile()));
    gl->addWidget( mUmountButton, 2, 2 );

    mUmountLineEdit=new QLineEdit(mGroupBox);
    Q_CHECK_PTR(mUmountLineEdit);
    mUmountLineEdit->setMinimumWidth( fontMetrics().maxWidth()*10 );
    connect(mUmountLineEdit,SIGNAL(textChanged(const QString&)),
	    this,SLOT(umntCmdChanged(const QString&)));
    connect( mUmountLineEdit, SIGNAL(textChanged(const QString&)),
	     this,SLOT(slotChanged()));
    gl->addWidget( mUmountLineEdit, 2, 3 );

  }

  loadSettings();
  if(init)
  {
    applySettings();
    mDiskLookup.resize(0);
  }

  mGroupBox->setEnabled( false );
}


MntConfigWidget::~MntConfigWidget( void )
{
}


void MntConfigWidget::readDFDone( void )
{
  mInitializing = false;
  mList->clear();
  mDiskLookup.resize(mDiskList.count());

  int i=0;
  QListViewItem *item = 0;
  for( DiskEntry *disk=mDiskList.first(); disk!=0; disk=mDiskList.next(),++i )
  {
     item = new QListViewItem( mList, item, QString::null, disk->deviceName(),
      disk->mountPoint(), disk->mountCommand(), disk->umountCommand() );
     item->setPixmap( ICONCOL, SmallIcon( disk->iconName() ) );
     mDiskLookup[i] = item;
  }

  loadSettings();
  applySettings();
}


void MntConfigWidget::applySettings( void )
{
  mDiskList.applySettings();

  KConfig &config = *kapp->config();
  config.setGroup("MntConfig");
  if(GUI )
  {
   config.writeEntry("Width", width() );
   config.writeEntry("Height", height() );
  }
  config.sync();
}


void MntConfigWidget::loadSettings( void )
{
  KConfig &config = *kapp->config();
  if( mInitializing == false && GUI )
  {
    config.setGroup("MntConfig");
    if( isTopLevel() )
    {
      int w = config.readNumEntry("Width",this->width() );
      int h = config.readNumEntry("Height",this->height() );
      resize(w,h);
    }

    QListViewItem *item = mList->selectedItem();
    if( item != 0 )
    {
      clicked( item );
    }
  }
}


void MntConfigWidget::clicked( QListViewItem *item )
{
  mGroupBox->setEnabled( true );
  mGroupBox->setTitle( QString("%1: %2  %3: %4").
    arg(mList->header()->label(DEVCOL)).
    arg(item->text(DEVCOL)).
    arg(mList->header()->label(MNTPNTCOL)).
    arg(item->text(MNTPNTCOL)) );


  const QPixmap *pix = item->pixmap(ICONCOL);
  if( pix != 0 )
  {
    mIconButton->setPixmap( *pix );
  }

  for(unsigned i=0 ; i < mDiskList.count() ; ++i) 
    {
      if (mDiskLookup[i] == item) 
	{
	  DiskEntry *disk = mDiskList.at(i);
	  if( disk != 0 )
	    {
	      mIconLineEdit->setText( disk->iconName() );
	    }
	  break;
	}
    }
  mMountLineEdit->setText( item->text(MNTCMDCOL) );
  mUmountLineEdit->setText( item->text(UMNTCMDCOL) );
}


void MntConfigWidget::iconChangedButton(QString iconName)
{
  iconChanged(iconName);
}
void MntConfigWidget::iconChanged(const QString &iconName)
{
  if( iconName.findRev('_') == 0 ||
      (iconName.right(iconName.length()-iconName.findRev('_'))!="_mount" &&
      iconName.right(iconName.length()-iconName.findRev('_'))!="_unmount"))
    {
      QString msg = i18n(""
			 "This filename is not valid: %1\n"
			 "It must end with "
			 "\"_mount\" or \"_unmount\".").arg(iconName);
      KMessageBox::sorry( this, msg );
      return;
    }

  QListViewItem *item = mList->selectedItem();
  for(unsigned i=0 ; i < mDiskList.count() ; ++i) 
    {
      if (mDiskLookup[i] == item) 
	{
	  DiskEntry *disk = mDiskList.at(i);
	  if( disk != 0 )
	    {
	      disk->setIconName(iconName);
	      mIconLineEdit->setText(iconName);
	      KIconLoader &loader = *KGlobal::iconLoader();
	      item->setPixmap( ICONCOL, loader.loadIcon( iconName, KIcon::Small));
	    }
	  break;
	}
    }
}


void MntConfigWidget::selectMntFile()
{
  KURL url = KFileDialog::getOpenURL( "","*", this );

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
  KURL url = KFileDialog::getOpenURL( "", "*", this );

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
  QListViewItem *item = mList->selectedItem();
  for(unsigned  i=0 ; i < mDiskList.count() ; ++i) 
    {
      if (mDiskLookup[i] == item)
	{
	  DiskEntry *disk = mDiskList.at(i);
	  if( disk != 0 )
	    {
	      disk->setMountCommand(data);
	      item->setText( MNTCMDCOL, data );
	    }
	  break;
	}
    }
}


void MntConfigWidget::umntCmdChanged( const QString &data )
{
  QListViewItem *item = mList->selectedItem();
  for(unsigned i=0 ; i < mDiskList.count() ; ++i) 
    {
    if (mDiskLookup[i] == item) 
      {
	DiskEntry *disk = mDiskList.at(i);
	if( disk != 0 )
	  {
	    disk->setUmountCommand(data);
	    item->setText( UMNTCMDCOL, data );
	  }
	break;
      }
    }
}


void MntConfigWidget::closeEvent(QCloseEvent *)
{
};

void MntConfigWidget::slotChanged()
{
  emit configChanged();
}

#include "mntconfig.moc"
