/*
**
** Copyright (C) 1998 by Michael Kropfberger <michael.kropfberger@gmx.net>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to <kde-devel@kde.org>
*/

#ifndef CSTDOPTION_H
#define CSTDOPTION_H

#include "kdfprivate_export.h"

#include <QString>

class KDFPRIVATE_EXPORT CStdOption
{
    public:
        CStdOption();
        ~CStdOption();

        void updateConfiguration();
        void writeConfiguration();
        void writeDefaultFileManager();

        QString fileManager() const;

        int     updateFrequency() const;
        bool    popupIfFull() const;
        bool    openFileManager() const;

        void setDefault( );
        void setFileManager( const QString &fileManager );
        void setUpdateFrequency( int frequency );
        void setPopupIfFull( bool popupIfFull );
        void setOpenFileManager( bool openFileManagerOnMount );

    private:
        static QString mDefaultFileManager;
        static int mDefaultUpdateFrequency;
        QString mFileManager;
        int     mUpdateFrequency;
        bool    mPopupIfFull;
        bool    mOpenFileManagerOnMount;
};

#endif

