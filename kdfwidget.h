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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __KDFWIDGET_H__
#define __KDFWIDGET_H__

#include <qarray.h>
#include <qglobal.h>
#include <qstring.h>


#include "disks.h"
#include "disklist.h"
#include "mntconfig.h"
#include "kdfconfig.h"

class CListView;
class COptionDialog;
class QListViewItem;
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







class KDFWidget : public KConfigWidget
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
    void setUpdateFreq( int freq );
    void invokeHTMLHelp( void );

  protected:
    void paintEvent( QPaintEvent * );
    void timerEvent( QTimerEvent * );
    void closeEvent( QCloseEvent * );

  private:
    void makeColumns( void );
    void updatePixmaps( void );
    DiskEntry *selectedDisk( QListViewItem *item=0 );

  private:
    bool        readingDF;

    QArray<CTabEntry*> mTabProp;

    CListView     *mList;
    COptionDialog *mOptionDialog;
    KPopupMenu    *mPopup;

    DiskList    diskList;
    int         updateFreq;
    QString     fileMgr;

    bool mIsTopLevel;
    bool mPopupIfFull;
    bool openFileMgrOnMount;
 
};


#endif
