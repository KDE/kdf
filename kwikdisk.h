/*
  kwikdisk.h - KDiskFree

  written 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
   
  */

#ifndef __KDF_H__
#define __KDF_H__

// A Qt define in qmenudata.h
#define INCLUDE_MENUITEM_DEF 1

#include <qintdict.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qtooltip.h>

#include <kmainwindow.h>

#include "disklist.h"
#include "kdfconfig.h"
#include "mntconfig.h"
#include "stdoption.h"

class COptionDialog;

/***************************************************************/
class MyToolTip : public QToolTip
{
  public:
    MyToolTip( QWidget *parent, QToolTipGroup *group=0 );
    virtual ~MyToolTip( void );

    void setPossibleTip( const QRect &rect, const QString &text );
    void setTipping( bool enableTipping );

  protected:
    void maybeTip( const QPoint &p ); 

  private:
    QRect   mRect;
    QString mText;
    bool    mEnableTipping;
};

/***************************************************************/
class MyPopupMenu : public QPopupMenu
{ 
  Q_OBJECT

  public:
    MyPopupMenu(QWidget *parent=0, const char *name=0);
    ~MyPopupMenu( void );

    void setToolTip(int id, const QString & text );
    QRect itemRectangle( int id );
    int activeIndex( void );

  private slots:
    void registerActiveItem( int id );
    void aboutToHide();
    void aboutToShow();

  private:
    int mCurrentId;
    int mCurrentIndex;
    MyToolTip *mToolTip;
    QIntDict<QString> mToolTipStrings;
};


/***************************************************************/
class DockWidget : public QLabel
{ 
  Q_OBJECT

  public:
    DockWidget(QWidget *parent=0, const char *name=0);
    ~DockWidget( void );

  public slots:
    void loadSettings();

  private slots:
    void criticallyFull(DiskEntry *disk);
    void toggleMount( void );
    void settingsBtnClicked( void );
    void invokeHelp( void );
    void quit( void );
    void startKDF( void );
    void updateDF( void );
    void updateDFDone( void );
    void setUpdateFrequency( int frequency );
    void sysCallError(DiskEntry *disk, int err_no);

  protected:
    void mousePressEvent(QMouseEvent *);
    void  timerEvent( QTimerEvent * );

  private:
    DiskEntry *selectedDisk( void );
    void showPopupMenu( void );

  signals:
    void quitProgram( void );

  private:
    MyPopupMenu   *mPopupMenu;
    COptionDialog *mOptionDialog;
    bool mReadingDF;
    bool mDirty;         
    DiskList mDiskList;
    CStdOption mStd;
};


/***************************************************************/
class KwikDiskTopLevel : public KMainWindow
{ 
  Q_OBJECT

  public:
    KwikDiskTopLevel(QWidget *parent=0, const char *name=0);
    ~KwikDiskTopLevel( void );

  protected slots:
    virtual bool queryExit( void );

  private:
    DockWidget   *mDockIcon;
};
/***************************************************************/


#endif
