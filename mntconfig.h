/*
 * mntconfig.h
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


#ifndef __MNTCONFIG_H__
#define __MNTCONFIG_H__

#include <kconfig.h>
#include <kcontrol.h>
#include <kiconloader.h>

#include "disks.h"
#include "disklist.h"

class QGroupBox;
class QPushButton;
class QRadioButton;
class QLineEdit;

class KTabListBox;


/**************************************************************/

class MntConfigWidget : public KConfigWidget
{
  Q_OBJECT
public:
  MntConfigWidget( QWidget *parent=0, const char *name=0, bool init=FALSE);
  ~MntConfigWidget();

public slots:
  void loadSettings();
  void applySettings();
  
private slots:
  void readDFDone();
  void clicked(int index, int column);
  void selectIcon();
  void selectMntFile();
  void selectUmntFile();
  void iconChanged(const QString&);
  void mntCmdChanged(const QString&);
  void umntCmdChanged(const QString&);

protected:
  void resizeEvent( QResizeEvent * );       
  void closeEvent( QCloseEvent * );

private:
  KIconLoader       *loader;
  KConfig           *config;
  QStrList          tabHeaders;
  QArray<int>       tabWidths;
  KTabListBox      *tabList;

  int               actRow;
  DiskEntry        *actDisk;
  QGroupBox        *boxActDev;
  QPushButton      *btnActIcon;
  QLineEdit        *qleIcon;
  QLineEdit        *qleMnt;
  QLineEdit        *qleUmnt;
  QPushButton      *btnMntFile;
  QPushButton      *btnUmntFile;

  DiskList          diskList;
  bool              initializing;
};


#endif
