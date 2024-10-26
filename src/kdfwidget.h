/*
    SPDX-FileCopyrightText: 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDFWIDGET_H
#define KDFWIDGET_H

#include "disklist.h"
#include "disks.h"
#include "kdfconfig.h"
#include "kdfitemdelegate.h"
#include "kdfprivate_export.h"
#include "kdfsortproxymodel.h"
#include "mntconfig.h"
#include "stdoption.h"

class QTreeView;
class QTimer;
class COptionDialog;

class QStandardItemModel;

/* Column class (id, column number, column name) */
class Column
{
public:
    Column(const QString &_name, const QString &_columnName, int _defaultWidth, int _number)
    {
        name = _name;
        columnName = _columnName;
        defaultWidth = _defaultWidth;
        number = _number;
    }

    QString name;
    QString columnName;
    int defaultWidth;
    int number;
};

class KDFPRIVATE_EXPORT KDFWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KDFWidget(QWidget *parent = nullptr, bool init = false);
    ~KDFWidget() override;

    enum ColumnNumber {
        IconCol = 0,
        DeviceCol = 1,
        TypeCol = 2,
        SizeCol = 3,
        MountPointCol = 4,
        FreeCol = 5,
        FullCol = 6,
        UsageBarCol = 7
    };

public Q_SLOTS:
    void settingsChanged(void);
    void loadSettings(void);
    void applySettings(void);
    void updateDF(void);
    void updateDFDone(void);
    void settingsBtnClicked(void);

private Q_SLOTS:
    void criticallyFull(DiskEntry *disk);
    void contextMenuRequested(const QPoint &p);
    void setUpdateFrequency(int frequency);
    void invokeHelp(void);

protected:
    void timerEvent(QTimerEvent *) override;
    void closeEvent(QCloseEvent *) override;

private:
    void makeColumns(void);
    DiskEntry *selectedDisk(const QModelIndex &index);
    QIcon generateIcon(const QString &iconName, bool mode, bool mounted);

    bool readingDF;
    COptionDialog *mOptionDialog = nullptr;
    QMenu *mPopup = nullptr;
    QTimer *mTimer = nullptr;
    DiskList mDiskList;
    bool mIsTopLevel;
    CStdOption mStd;

    QTreeView *m_listWidget = nullptr;
    KDFItemDelegate *m_itemDelegate = nullptr;
    QList<Column> m_columnList;

    QStandardItemModel *m_listModel = nullptr;
    KDFSortFilterProxyModel *m_sortModel = nullptr;
};

#endif
