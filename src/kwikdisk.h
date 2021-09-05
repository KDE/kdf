/*
kwikdisk.cpp - KDiskFree

SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

SPDX-License-Identifier: GPL-2.0-or-later

*/

// With changes by Espen Sand and Stanislav Karchebny.

#ifndef KWIKDISK_H
#define KWIKDISK_H

#include "disklist.h"
#include "stdoption.h"
#include "optiondialog.h"

#include <KStatusNotifierItem>

class QActionGroup;

/**
* @short Application Main Window (however in Tray)
* @version 0.2
*/

class KwikDisk : public KStatusNotifierItem
{
    Q_OBJECT

    public:
        KwikDisk();

    public Q_SLOTS:
        void activate(const QPoint &pos) override;

    private Q_SLOTS:
        void updateDFDone();
        void criticallyFull(DiskEntry*);
        void toggleMount(QAction*);
        void loadSettings();
        void changeSettings();
        void startKDF();
        void invokeHelp();
        void clearDeviceActions();

    private:
        void timerEvent(QTimerEvent *) override;
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

