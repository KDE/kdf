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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
   
  */

//
// 1999-12-05 Espen Sand
// Modified to use KCModule instead of the old and obsolete
// KControlApplication
//

#include "kcmdf.h"

#include <QtGui/QVBoxLayout>

#include <kdialog.h>
#include <klocale.h>
#include <kcomponentdata.h>

KDiskFreeWidget::KDiskFreeWidget( const KComponentData &inst, QWidget *parent )
        : KCModule( inst, parent )
{
    setButtons(Help);

    QVBoxLayout *topLayout = new QVBoxLayout( this );
    topLayout->setSpacing( KDialog::spacingHint() );
    topLayout->setMargin( 0 );

    mKdf = new KDFWidget( this, false );
    topLayout->addWidget( mKdf );
}

KDiskFreeWidget::~KDiskFreeWidget()
{
    mKdf->applySettings();
}

QString KDiskFreeWidget::quickHelp() const
{
    return i18n("A right mouse button click opens a context menu to mount/unmount a device"
                " or to open it in the file manager.");
}

extern "C"
{
    KDE_EXPORT KCModule* create_kdf( QWidget *parent, const char * /*name*/ )
    {
        KComponentData inst("kdf");
        return new KDiskFreeWidget( inst , parent );
    }
}

#include "kcmdf.moc"

