#include <kapplication.h>
#include <kconfig.h>

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
  KConfig &config = *kapp->config();
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
  KConfig &config = *kapp->config();
  config.setGroup("KDFConfig");
  config.writeEntry( "UpdateFrequency", mUpdateFrequency );
  config.writePathEntry( "FileManagerCommand", mFileManager );
  config.writeEntry( "PopupIfFull", mPopupIfFull );
  config.writeEntry( "OpenFileMgrOnMount", mOpenFileManagerOnMount );
  config.sync();
}  


void CStdOption::writeDefaultFileManager( void )
{
  KConfig &config = *kapp->config();
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









