/*
    SPDX-FileCopyrightText: 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

//
// 1999-12-05 Espen Sand
// Modified to use KCModule instead of the old and obsolete
// KControlApplication
//

#include "kcmdf.h"

#include <KPluginFactory>
#include <QVBoxLayout>

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
