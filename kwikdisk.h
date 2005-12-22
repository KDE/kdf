/*
  kwikdisk.cpp - KDiskFree

  Copyright (C) 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  */

// With changes by Espen Sand and Stanislav Karchebny.

#ifndef _KWIKDISK_H_
#define _KWIKDISK_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disklist.h"
#include "stdoption.h"
#include "optiondialog.h"

#include <kmainwindow.h>
#include <ksystemtray.h>

/**
* @short Application Main Window (however in Tray)
* @version 0.2
*/
class KwikDisk : public KSystemTray
{
   Q_OBJECT
   public:
      KwikDisk();

   private slots:
      void updateDFDone();
      void criticallyFull(DiskEntry*);
      void toggleMount(int);
      void loadSettings();
      void changeSettings();
      void startKDF();
      void invokeHelp();
      void aboutToHide();

   private:
      void mousePressEvent(QMouseEvent *);
      void timerEvent(QTimerEvent *);
      void enterEvent(QEvent *);
      void leaveEvent(QEvent *);

      void setUpdateFrequency(int frequency);

      void updateDF();

   private:
      DiskList       m_diskList;
      CStdOption     m_options;
      bool           m_readingDF;
      bool           m_dirty;
      bool           m_menuVisible;
      bool           m_inside;
      COptionDialog *m_optionDialog;
};

#endif // _KWIKDISK_H_
