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


#include <kdialog.h>

#include <qlayout.h>

#include "kcmdf.h"

KDiskFreeWidget::KDiskFreeWidget( QWidget *parent, const char *name )
 : KCModule( parent, name )
{
  setButtons(Help);

  QVBoxLayout *topLayout = new QVBoxLayout( this );

  mPage = new QFrame( this, "page" );
  topLayout->addWidget( mPage, 10 );
  QVBoxLayout *vbox = new QVBoxLayout( mPage, KDialog::spacingHint() );
  mKdf = new KDFWidget( mPage, "kdf", false );
  vbox->addWidget( mKdf, 10 );
}

KDiskFreeWidget::~KDiskFreeWidget()
{
  mKdf->applySettings();
}

extern "C"
{
  KCModule* create_kdf( QWidget *parent, const char * /*name*/ )
  {
    return new KDiskFreeWidget( parent , "kdf" );
  }
}

#include "kcmdf.moc"
