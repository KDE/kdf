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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */

#ifndef __KDF_H__
#define __KDF_H__

#include <qtooltip.h>
#include <qintdict.h>
#include <qtabdialog.h>

#include <kwm.h>
#include <ktopwidget.h>

#include "mntconfig.h"
#include "kdfconfig.h"
#include "disklist.h"


/***************************************************************/
class MyToolTip : public QToolTip
{

public:
  MyToolTip(QWidget *parent, QToolTipGroup * group= 0 );
  //  ~MyToolTip() { delete text; };
  void setPossibleTip(const QRect &rect,const char *text);
  void setTipping(bool is_tipping) {tipping=is_tipping; };

protected:
  void maybeTip(const QPoint &p); 

private:
  QRect rect;
  const char *text;
  bool  tipping;
};

/***************************************************************/
class MyPopupMenu : public QPopupMenu
{ Q_OBJECT

public:
  MyPopupMenu(QWidget *parent=0, const char *name=0);
  ~MyPopupMenu() { delete toolTip; };
  void setToolTip(int id, const char *text);
  int cellHeight(int id) { return QPopupMenu::itemHeight(id); };
  int totalWidth() { return QPopupMenu::width(); };
private slots:

protected:
   void mouseMoveEvent(QMouseEvent *);

private:
   int              toolTipRow;
   MyToolTip        *toolTip;
   QIntDict<char>   toolTips;
};

/***************************************************************/
class DockWidget : public QLabel
{ Q_OBJECT

public:
  DockWidget(QWidget *parent=0, const char *name=0);
  ~DockWidget() { delete clickMenu; };

public slots:
  void loadSettings();
  void applySettings();

private slots:
   void confLoadSettings();
   void confApplySettings();
   void criticallyFull(DiskEntry *disk);
   void toggleMount();
   void settingsBtnClicked();
   void invokeHTMLHelp();
   void quit();
   void startKDF();
   void updateDF();
   void updateDFDone();
   void setUpdateFreq(int freq);
   void sysCallError(DiskEntry *disk, int errno);

protected:
  void mousePressEvent(QMouseEvent *);
  void  timerEvent( QTimerEvent * );

private:
    KConfig           *config;
    MyPopupMenu      *clickMenu;
    DiskList         diskList;
    bool              readingDF;

    QTabDialog       *tabconf;
    KDFConfigWidget  *kdfconf;
    MntConfigWidget  *mntconf;

   QString          fileMgr;
   int               updateFreq;
   bool             popupIfFull;
   bool             openFileMgrOnMount;
};


/***************************************************************/
class KwikDiskTopLevel : public KTMainWindow
{ Q_OBJECT

public:
  KwikDiskTopLevel(QWidget *parent=0, const char *name=0);
  ~KwikDiskTopLevel() { delete dockIcon; };

private:
  DockWidget   *dockIcon;
};
/***************************************************************/


#endif
