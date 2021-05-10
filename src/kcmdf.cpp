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

#include <QVBoxLayout>
#include <KPluginFactory>
#include <kpluginfactory.h>

K_PLUGIN_CLASS_WITH_JSON(KDiskFreeWidget, "kcmdf.json")

KDiskFreeWidget::KDiskFreeWidget( QWidget *parent, const QVariantList &args )
        : KCModule( parent, args )
{
    setButtons(Help);

    QVBoxLayout *topLayout = new QVBoxLayout( this );
    topLayout->setContentsMargins({});

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

#include "kcmdf.moc"
