/*
  kcmdf.cpp - KcmDiskFree

  Copyright (C) 1998 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
   
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

  QVBoxLayout *topLayout = new QVBoxLayout( this, 0, KDialog::spacingHint() );

  mKdf = new KDFWidget( this, "kdf", false );
  topLayout->addWidget( mKdf );
}

KDiskFreeWidget::~KDiskFreeWidget()
{
  mKdf->applySettings();
}

QString KDiskFreeWidget::quickHelp() const
{
    return i18n("<h3>Hardware Information</h3><br> All the information modules return information"
    " about a certain aspect of your computer hardware or your operating system."
    " Not all modules are available on all hardware architectures and/or operating systems.");
}

extern "C"
{
  KCModule* create_kdf( QWidget *parent, const char * /*name*/ )
  {
    return new KDiskFreeWidget( parent , "kdf" );
  }
}

#include "kcmdf.moc"
