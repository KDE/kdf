/*
*   Copyright (C) 1999  Espen Sand, espen@kde.org
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
*/

#ifndef COPTIONDIALOG_H
#define COPTIONDIALOG_H

#include "kdf_export.h"

#include <kpagedialog.h>

class KDFConfigWidget;
class MntConfigWidget;

class KDF_EXPORT COptionDialog : public KPageDialog
{
    Q_OBJECT

    public:
        explicit COptionDialog( QWidget *parent=0 );
        ~COptionDialog( void );

    protected Q_SLOTS:
        void slotOk( void );
        void slotApply( void );
        void slotChanged();
        void slotHelp();

    Q_SIGNALS:
        void valueChanged( void );

    private:
        KDFConfigWidget *mConf;
        MntConfigWidget *mMnt;
        bool dataChanged;
};

#endif

