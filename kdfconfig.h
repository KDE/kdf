/*
 * kdfconfig.h
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
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


#ifndef __KDFCONFIG_H__
#define __KDFCONFIG_H__


#include <qlcdnum.h> 
#include <qlabel.h> 
#include <qbttngrp.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qcheckbox.h>
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

/**************************************************************/

class KDFConfigWidget : public KConfigWidget
{
  Q_OBJECT
public:
  KDFConfigWidget( QWidget *parent=0, const char *name=0, bool init=FALSE);
  ~KDFConfigWidget();

public slots:
  void loadSettings();
  void applySettings();
  void defaultsBtnClicked();

private slots:
   void toggleColumnVisibility(int column);
   void toggleColumnVisibility(int,int column)
         { toggleColumnVisibility(column); }; //overloaded

protected:
  void resizeEvent( QResizeEvent * );       
  void closeEvent( QCloseEvent * );

private:
  KConfig           *config;
  QStrList          tabHeaders;
  QArray<int>       tabWidths;


  KTabListBox      *confTabList;
  QLabel           *freqLabel
                  ,*fileMgrLabel;
  QScrollBar       *freqScroll;
  QLCDNumber       *freqLCD;
  QLineEdit        *fileMgrEdit;
  QCheckBox        *cbPopupIfFull;
  QCheckBox        *cbOpenFileMgrOnMount;

  bool             isTopLevel;
 
};


#endif
