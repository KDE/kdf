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

#include "stdoption.h"

#include <kconfiggroup.h>
#include <ksharedconfig.h>

#include <QString>

QString CStdOption::mDefaultFileManager = QStringLiteral( "dolphin %m" );
int CStdOption::mDefaultUpdateFrequency = 60;

CStdOption::CStdOption()
{
    setDefault();
}


CStdOption::~CStdOption()
{
}


void CStdOption::updateConfiguration()
{
    KConfigGroup config(KSharedConfig::openConfig(), "KDFConfig");
    mFileManager = config.readPathEntry(
                       "FileManagerCommand", mDefaultFileManager );
    mUpdateFrequency = config.readEntry(
                           "UpdateFrequency", mDefaultUpdateFrequency );
    mPopupIfFull = config.readEntry(
                       "PopupIfFull", true );
    mOpenFileManagerOnMount = config.readEntry(
                                  "OpenFileMgrOnMount", false );
}


void CStdOption::writeConfiguration()
{
    KConfigGroup config(KSharedConfig::openConfig(), "KDFConfig");
    config.writeEntry( "UpdateFrequency", mUpdateFrequency );
    config.writePathEntry( "FileManagerCommand", mFileManager );
    config.writeEntry( "PopupIfFull", mPopupIfFull );
    config.writeEntry( "OpenFileMgrOnMount", mOpenFileManagerOnMount );
    config.sync();
}


void CStdOption::writeDefaultFileManager()
{
    KConfigGroup config(KSharedConfig::openConfig(), "KDFConfig");
    config.writePathEntry( "FileManagerCommand", mDefaultFileManager );
    config.sync();
}



QString CStdOption::fileManager() const
{
    return( mFileManager );
}


int CStdOption::updateFrequency() const
{
    return( mUpdateFrequency );
}


bool CStdOption::popupIfFull() const
{
    return( mPopupIfFull );
}


bool CStdOption::openFileManager() const
{
    return( mOpenFileManagerOnMount );
}


void CStdOption::setDefault()
{
    mFileManager     = mDefaultFileManager;
    mUpdateFrequency = mDefaultUpdateFrequency;
    mPopupIfFull     = true;
    mOpenFileManagerOnMount = false;
}


void CStdOption::setFileManager( const QString &fileManager )
{
    mFileManager = fileManager;
}


void CStdOption::setUpdateFrequency( int frequency )
{
    mUpdateFrequency = frequency;
}


void CStdOption::setPopupIfFull( bool popupIfFull )
{
    mPopupIfFull = popupIfFull;
}


void CStdOption::setOpenFileManager( bool openFileManagerOnMount )
{
    mOpenFileManagerOnMount = openFileManagerOnMount;
}

