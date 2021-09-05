/*
    SPDX-FileCopyrightText: 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
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

