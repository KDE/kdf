/*
  kcmdf.cpp - KcmDiskFree

  written 1998 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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
// 1999-12-05 Espen Sand 
// Modified to use KCModule instead of the old and obsolete 
// KControlApplication
//


#include <kapp.h>
#include <kdialog.h>
#include <kcmdlineargs.h>
#include <klocale.h>

#include <qframe.h>
#include <qlayout.h>

#include "kcmdf.h"
#include "kcmodule.h"

KDiskFreeWidget::KDiskFreeWidget( QWidget *parent, const char *name )
 : KCModule( parent, name )
{
  setButtons(Help|Default|Cancel|Apply|Ok);

  QVBoxLayout *topLayout = new QVBoxLayout( this );
  mTab = new QTabWidget( this );
  if( mTab == 0 ) { return; }
  topLayout->addWidget( mTab, 10 );

  mPage[0] = new QFrame( mTab, "page" );
  CHECK_PTR( mPage[0] );
  mTab->addTab( mPage[0], i18n("&KDiskFree") );
  QVBoxLayout *vbox = new QVBoxLayout( mPage[0], KDialog::spacingHint() );
  mKdf = new KDFWidget( mPage[0], "kdf", false );
  vbox->addWidget( mKdf, 10 );

  mPage[1] = new QFrame( mTab, "page" );
  CHECK_PTR( mPage[1] );
  mTab->addTab( mPage[1], i18n("&General Settings") );
  vbox = new QVBoxLayout( mPage[1], KDialog::spacingHint() );
  mMcw = new KDFConfigWidget( mPage[1], "kcw", false );
  vbox->addWidget( mMcw, 10 );

  mPage[2] = new QFrame( mTab, "page" );
  CHECK_PTR( mPage[2] );
  mTab->addTab( mPage[2], i18n("&Mount Commands") );
  vbox = new QVBoxLayout( mPage[2], KDialog::spacingHint() );
  mKcw = new MntConfigWidget( mPage[2], "mcw", false );
  vbox->addWidget( mKcw, 10 );
}


void KDiskFreeWidget::load( void )
{
  //
  // 1999-12-05 Espen Sand
  // I don't use this one because 1) The widgets will do a 
  // loadSettings() on startup and 2) Reset button is not used.
  //
}


void KDiskFreeWidget::save( void )
{
  int pn = pageNumber();
  if( pn == 0 )
  {
    mKdf->applySettings();
  }
  else if( pn == 1 )
  {
    mMcw->applySettings();
  }
  else if( pn == 2 )
  {
    mKcw->applySettings();
  }
}


void KDiskFreeWidget::defaults( void )
{
  int pn = pageNumber();
  if( pn == 0 )
  {
    mKdf->loadSettings();
  }
  else if( pn == 1 )
  {
    mMcw->loadSettings();
  }
  else if( pn == 2 )
  {
    mKcw->loadSettings();
  }
}


void KDiskFreeWidget::hideEvent( QHideEvent * )
{
  kapp->quit();
}


int KDiskFreeWidget::pageNumber( void )
{
  QWidget *w = mTab->currentPage();
  if( w == 0 ) { return(-1); }

  for( int i=0; i<3; i++ )
  { 
    if( w == mPage[i] )
    {
      return(i);
    }
  }
  return(-1);
}


extern "C"
{
  KCModule* create_kdf( QWidget *parent, const char * /*name*/ )
  {
    KGlobal::locale()->insertCatalogue("kdf");
    return new KDiskFreeWidget( parent );
  }
}

#include "kcmdf.moc"
