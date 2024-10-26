/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MNTCONFIG_H
#define MNTCONFIG_H

#include "disklist.h"
#include "ui_mntconfig.h"

#include <QWidget>

/**************************************************************/

class MntConfigWidget : public QWidget, private Ui_MntConfigWidget
{
    Q_OBJECT

public:
    enum ColType {
        IconCol = 0,
        DeviceCol = 1,
        MountPointCol = 2,
        MountCommandCol = 3,
        UmountCommandCol = 4
    };

    explicit MntConfigWidget(QWidget *parent = nullptr, bool init = false);
    ~MntConfigWidget() override;

public Q_SLOTS:
    void loadSettings(void);
    void applySettings(void);

protected Q_SLOTS:
    void slotChanged();

private Q_SLOTS:
    void readDFDone(void);
    void clicked(QTreeWidgetItem *, int);
    void selectMntFile(void);
    void selectUmntFile(void);
    void iconChangedButton(const QString &);
    void iconChanged(const QString &);
    void iconDefault();
    void mntCmdChanged(const QString &);
    void umntCmdChanged(const QString &);

    DiskEntry *selectedDisk(QTreeWidgetItem *);

protected:
    void closeEvent(QCloseEvent *) override;

private:
    DiskList mDiskList;
    bool mInitializing;

Q_SIGNALS:
    void configChanged();
};

#endif
