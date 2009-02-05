/*
kcmdf.h - KDiskFree

Copyright (C) 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>

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


#ifndef KCMDF_H
#define KCMDF_H

#include <kcmodule.h>

#include "kdfwidget.h"
#include "kdfconfig.h"
#include "mntconfig.h"

class KDiskFreeWidget : public KCModule
{
    Q_OBJECT
    
    public:
        explicit KDiskFreeWidget( const KComponentData &inst, QWidget *parent = 0  );
        ~KDiskFreeWidget();
        
        QString quickHelp() const;
    
    private:
        KDFWidget *mKdf;
        KDFConfigWidget *mMcw;
        MntConfigWidget *mKcw;
};

#endif

