/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/


#ifndef KCMDF_H
#define KCMDF_H
#include "kcmutils_version.h"
#include <KCModule>

#include "kdfwidget.h"
#include "kdfconfig.h"
#include "mntconfig.h"

class KDiskFreeWidget : public KCModule
{
    Q_OBJECT

    public:
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
        explicit KDiskFreeWidget( QWidget *parent = nullptr, const QVariantList &args = {});
#else
        explicit KDiskFreeWidget(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
#endif
        ~KDiskFreeWidget() override;
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 240, 0)
        QString quickHelp() const override;
#endif

    private:
        KDFWidget *mKdf;
        KDFConfigWidget *mMcw;
        MntConfigWidget *mKcw;
};

#endif

