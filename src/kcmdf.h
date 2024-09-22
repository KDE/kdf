/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/


#ifndef KCMDF_H
#define KCMDF_H
#include <KCModule>

#include "kdfwidget.h"
#include "kdfconfig.h"
#include "mntconfig.h"

class KPluginMetaData;
class KDiskFreeWidget : public KCModule
{
    Q_OBJECT

    public:
        explicit KDiskFreeWidget(QObject *parent, const KPluginMetaData &data);

    private:
        KDFWidget *mKdf;
        KDFConfigWidget *mMcw;
        MntConfigWidget *mKcw;
};

#endif

