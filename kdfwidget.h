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


#include <qlcdnum.h> 
#include <qlabel.h> 
#include <qbttngrp.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qlist.h>
#include <qlined.h>
#include <qlayout.h>
#include <qscrbar.h>
#include <qarray.h>
#include <qtabdialog.h>
 
#include <kapp.h>
#include <kslider.h>
#include <kcontrol.h>
#include <ktablistbox.h>

// defines the os-type
#include <qglobal.h>

#include "disks.h"
#include "disklist.h"
#include "mntconfig.h"
#include "kdfconfig.h"

/**************************************************************/

class KDFWidget : public KConfigWidget
{
  Q_OBJECT
public:
  KDFWidget( QWidget *parent=0, const char *name=0, bool init=FALSE);
  ~KDFWidget();

public slots:
  void loadSettings();
  void applySettings();
  void updateDF();
  void updateDFDone();
  void settingsBtnClicked();
  
private slots:
   void confLoadSettings();
   void confApplySettings();
   void criticallyFull(DiskEntry *disk);
   void toggleColumnVisibility(int column);
   void toggleColumnVisibility(int,int column)
         { toggleColumnVisibility(column); }; //overloaded
   void popupMenu(int row,int column);
   void setUpdateFreq(int freq);
   void invokeHTMLHelp() { kapp->invokeHTMLHelp("kcontrol/kdf/index.html",""); };


protected:
  void paintEvent( QPaintEvent * );       
  void resizeEvent( QResizeEvent * );       
  void  timerEvent( QTimerEvent * );
  void closeEvent( QCloseEvent * );

private:
  void updatePixmaps();

  KConfig           *config;
  QTabDialog       *tabconf;
  MntConfigWidget   *mntconf;
  KDFConfigWidget   *kdfconf;

  bool              readingDF;
  QStrList          tabHeaders;
  QArray<int>       tabWidths;

  KTabListBox      *tabList;
  DiskList          diskList;
  int               updateFreq;
  QString          fileMgr;

  bool             isTopLevel;
  bool             popupIfFull;
  bool             openFileMgrOnMount;
 
};


#endif
