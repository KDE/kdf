/*
kwikdisk.cpp - KDiskFree

Copyright (C) 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>
              2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

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

#ifndef __KWIKDISK_H__
#define __KWIKDISK_H__

#include <ksystemtrayicon.h>

#include "disklist.h"
#include "stdoption.h"
#include "optiondialog.h"

class QActionGroup;

/**
* @short Application Main Window (however in Tray)
* @version 0.2
*/

class KwikDisk : public KSystemTrayIcon
{
    Q_OBJECT
    
    public:
        KwikDisk();
    
    private Q_SLOTS:
        void updateDFDone();
        void criticallyFull(DiskEntry*);
        void toggleMount(QAction*);
        void loadSettings();
        void changeSettings();
        void startKDF();
        void invokeHelp();
        void slotActivated(QSystemTrayIcon::ActivationReason reason);
        void clearDeviceActions();
    
    private:
        void timerEvent(QTimerEvent *);
        void enterEvent(QEvent *);
        void leaveEvent(QEvent *);
        
        void setUpdateFrequency(int frequency);
        
        void updateDF();
    
        DiskList       m_diskList;
        CStdOption     m_options;
        bool           m_readingDF;
        bool           m_dirty;
        bool           m_menuVisible;
        bool           m_inside;
        COptionDialog *m_optionDialog;
        QActionGroup * m_actionGroup;
        QAction * m_actionSeparator;
};

#endif // _KWIKDISK_H_

