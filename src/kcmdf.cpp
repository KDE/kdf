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

KDiskFreeWidget::KDiskFreeWidget(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    setButtons(Help);
    QVBoxLayout *topLayout = new QVBoxLayout(widget());
    topLayout->setContentsMargins({});
    mKdf = new KDFWidget(widget(), false);
    topLayout->addWidget(mKdf);
}

KDiskFreeWidget::~KDiskFreeWidget()
{
    mKdf->applySettings();
}
#include "kcmdf.moc"

#include "moc_kcmdf.cpp"
