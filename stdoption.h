/*
**
** Copyright (C) 1998 by Michael Kropfberger
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
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef _STD_OPTION_H_
#define _STD_OPTION_H_

#include <qstring.h>

class CStdOption
{
  public:
    CStdOption( void );
    ~CStdOption( void );

    void updateConfiguration( void );
    void writeConfiguration( void );
    void writeDefaultFileManager( void );

    QString fileManager( void );

    int     updateFrequency( void );
    bool    popupIfFull( void );
    bool    openFileManager( void );

    void setDefault( void );
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









