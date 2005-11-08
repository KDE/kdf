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
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/


#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>

#include "stdoption.h"

QString CStdOption::mDefaultFileManager = "kfmclient openURL %m";
int CStdOption::mDefaultUpdateFrequency = 60;

CStdOption::CStdOption( void )
{
  setDefault();
}


CStdOption::~CStdOption( void )
{
}


void CStdOption::updateConfiguration( void )
{
  KConfig &config = *KGlobal::config();
  config.setGroup("KDFConfig");
  mFileManager = config.readPathEntry(
    "FileManagerCommand", mDefaultFileManager );
  mUpdateFrequency = config.readNumEntry( 
    "UpdateFrequency", mDefaultUpdateFrequency );
  mPopupIfFull = config.readBoolEntry( 
    "PopupIfFull", true );
  mOpenFileManagerOnMount = config.readBoolEntry( 
    "OpenFileMgrOnMount", false );
}


void CStdOption::writeConfiguration( void )
{
  KConfig &config = *KGlobal::config();
  config.setGroup("KDFConfig");
  config.writeEntry( "UpdateFrequency", mUpdateFrequency );
  config.writePathEntry( "FileManagerCommand", mFileManager );
  config.writeEntry( "PopupIfFull", mPopupIfFull );
  config.writeEntry( "OpenFileMgrOnMount", mOpenFileManagerOnMount );
  config.sync();
}  


void CStdOption::writeDefaultFileManager( void )
{
  KConfig &config = *KGlobal::config();
  config.setGroup("KDFConfig");
  config.writePathEntry( "FileManagerCommand", mDefaultFileManager );
  config.sync();
}



QString CStdOption::fileManager( void )
{
  return( mFileManager );
}


int CStdOption::updateFrequency( void )
{
  return( mUpdateFrequency );
}


bool CStdOption::popupIfFull( void )
{
  return( mPopupIfFull ); 
}


bool CStdOption::openFileManager( void )
{
  return( mOpenFileManagerOnMount );
}


void CStdOption::setDefault( void )
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









