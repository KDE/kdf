/*
kdf.h - KDiskFree

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

#ifndef KDF_H
#define KDF_H

#include "kdfwidget.h"

#include <kxmlguiwindow.h>

/***************************************************************/
class KDFTopLevel : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        KDFTopLevel(QWidget *parent=0);
        ~KDFTopLevel() { }

    protected:
        void closeEvent(QCloseEvent *event);
    protected:
        KDFWidget* kdf;
};

#endif

