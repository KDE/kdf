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









