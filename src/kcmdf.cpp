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

K_PLUGIN_CLASS_WITH_JSON(KDiskFreeWidget, "kcm_kdf.json")

#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
KDiskFreeWidget::KDiskFreeWidget( QWidget *parent, const QVariantList &args )
        : KCModule( parent, args )
#else
KDiskFreeWidget::KDiskFreeWidget( QObject *parent, const KPluginMetaData &data)
        : KCModule( parent, data)
#endif
{
    setButtons(Help);
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    QVBoxLayout *topLayout = new QVBoxLayout( this );
#else
    QVBoxLayout *topLayout = new QVBoxLayout( widget() );
#endif
    topLayout->setContentsMargins({});
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
    mKdf = new KDFWidget( this, false );
#else
    mKdf = new KDFWidget( widget(), false );
#endif
    topLayout->addWidget( mKdf );
}

KDiskFreeWidget::~KDiskFreeWidget()
{
    mKdf->applySettings();
}
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
QString KDiskFreeWidget::quickHelp() const
{
    return i18n("A right mouse button click opens a context menu to mount/unmount a device"
                " or to open it in the file manager.");
}
#endif
#include "kcmdf.moc"

#include "moc_kcmdf.cpp"
