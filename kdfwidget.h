/*
 * kdfwidget.h
 *
 * Copyright (c) 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef __KDFWIDGET_H__
#define __KDFWIDGET_H__

#include <qmemarray.h>
#include <qglobal.h>
#include <qstring.h>
#include <qlistview.h>

#include "disks.h"
#include "disklist.h"
#include "mntconfig.h"
#include "kdfconfig.h"
#include "stdoption.h"

class CListView;
class COptionDialog;
class QTimer;
class KPopupMenu;

/**************************************************************/

class CTabEntry
{
  public:
    CTabEntry( const QString &res, const QString &name, bool visible,
	       uint width )
    {
      mRes     = res;
      mName    = name;
      mVisible = visible;
      mWidth   = width;
    };
    CTabEntry( void ) { }
    ~CTabEntry( void ) { }


    QString mRes;
    QString mName;
    bool mVisible;
    uint mWidth;
};

/**************************************************************/
//
// 2001-03-10 Walter F.J. Mueller <W.F.J.Mueller@gsi.de>
// The purpose of this class is:
// 1) overload the key() method to provide sorting keys so that the
//    columns with a numeric value are sorted by numerical value.
//

class CListViewItem : public QListViewItem
{
  public:
    CListViewItem ( CListView * parent, QListViewItem * after );
 
    virtual int compare ( QListViewItem* i, int col, bool) const;
    	
    void    setKeys ( int kb_size, int kb_avail, float percent_full ) ;

  private:
    int     size;
    int     avail;
    float   full;
};

/**************************************************************/

class KDFWidget : public QWidget
{
  Q_OBJECT

  public:
    enum ColId
    {
      iconCol   = 0,
      deviceCol = 1,
      typeCol   = 2,
      sizeCol   = 3,
      mntCol    = 4,
      freeCol   = 5,
      fullCol   = 6,
      usageCol  = 7
    };

  public:
    KDFWidget( QWidget *parent=0, const char *name=0, bool init=false);
    ~KDFWidget( void );

  public slots:
    void settingsChanged( void );
    void loadSettings( void );
    void applySettings( void );
    void updateDF( void );
    void updateDFDone( void );
    void settingsBtnClicked( void );
  
  private slots:
    void criticallyFull( DiskEntry *disk );
    void rightButtonPressed( QListViewItem *item, const QPoint &p, int );
    void rightButtonClicked( QListViewItem *item, const QPoint &p, int );
    void popupMenu( QListViewItem *item, const QPoint &p );
    void setUpdateFrequency( int frequency );
    void columnSizeChanged( int column, int, int newSize );
    void updateDiskBarPixmaps( void );
    void invokeHelp( void );

  protected:
    void timerEvent( QTimerEvent * );
    void closeEvent( QCloseEvent * );
    void resizeEvent( QResizeEvent * );

  private:
    void makeColumns( void );
    DiskEntry *selectedDisk( QListViewItem *item=0 );

  private:
    bool readingDF;
    QMemArray<CTabEntry*> mTabProp;
    CListView     *mList;
    COptionDialog *mOptionDialog;
    KPopupMenu    *mPopup;
    QTimer        *mTimer;
    DiskList   mDiskList;
    bool       mIsTopLevel;
    CStdOption mStd;
};


#endif
