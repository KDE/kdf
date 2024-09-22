/*
    SPDX-FileCopyrightText: 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDF_H
#define KDF_H

#include "kdfwidget.h"

#include <kxmlguiwindow.h>

/***************************************************************/
class KDFTopLevel : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        explicit KDFTopLevel(QWidget *parent= nullptr);
        ~KDFTopLevel() override { }

    protected:
        KDFWidget* kdf;
};

#endif

