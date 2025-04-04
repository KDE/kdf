/*
    SPDX-FileCopyrightText: 1998-2001 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

//
// 1999-11-29 Espen Sand
// Converted to QLayout and QListView + cleanups
// 1999-12-05 Espen Sand
// Usage bars should work again.
//

#include "kdfwidget.h"

#include "kdfprivate_debug.h"
#include "optiondialog.h"

#include <KConfigGroup>
#include <KDialogJobUiDelegate>
#include <KHelpClient>
#include <KMessageBox>
#include <KService>
#include <KShell>

#include <KIO/ApplicationLauncherJob>

#include <QAbstractEventDispatcher>
#include <QDesktopServices>
#include <QLayout>
#include <QMenu>
#include <QPainter>
#include <QProcess>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QUrl>

// This aren't used here...
// #define BAR_COLUMN   7
// #define FULL_PERCENT 95.0

#ifndef GUI_DEFINED
static bool GUI;
#define GUI_DEFINED
#endif

KDFWidget::KDFWidget(QWidget *parent, bool init)
    : QWidget(parent)
{
    connect(&mDiskList, &DiskList::readDFDone, this, &KDFWidget::updateDFDone);
    connect(&mDiskList, &DiskList::criticallyFull, this, &KDFWidget::criticallyFull);

    m_columnList.append(Column(QStringLiteral("Icon"), QLatin1String(""), 20, IconCol));
    m_columnList.append(Column(QStringLiteral("Device"), i18nc("Device of the storage", "Device"), 100, DeviceCol));
    m_columnList.append(Column(QStringLiteral("Type"), i18nc("Filesystem on storage", "Type"), 80, TypeCol));
    m_columnList.append(Column(QStringLiteral("Size"), i18nc("Total size of the storage", "Size"), 80, SizeCol));
    m_columnList.append(Column(QStringLiteral("MountPoint"), i18nc("Mount point of storage", "Mount Point"), 120, MountPointCol));
    m_columnList.append(Column(QStringLiteral("Free"), i18nc("Free space in storage", "Free"), 80, FreeCol));
    m_columnList.append(Column(QStringLiteral("Full%"), i18nc("Used storage space in %", "Full %"), 50, FullCol));
    m_columnList.append(Column(QStringLiteral("UsageBar"), i18nc("Usage graphical bar", "Usage"), 200, UsageBarCol));

    GUI = !init;
    if (GUI) {
        QVBoxLayout *topLayout = new QVBoxLayout(this);
        topLayout->setSpacing(0);
        topLayout->setContentsMargins(0, 0, 0, 0);

        m_listModel = new QStandardItemModel(this);
        m_sortModel = new KDFSortFilterProxyModel(this);
        m_sortModel->setSourceModel(m_listModel);

        m_listWidget = new QTreeView(this);
        m_listWidget->setModel(m_sortModel);

        m_itemDelegate = new KDFItemDelegate(m_listWidget);

        m_listWidget->setItemDelegate(m_itemDelegate);
        m_listWidget->setRootIsDecorated(false);
        m_listWidget->setSortingEnabled(true);
        m_listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        m_listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        topLayout->addWidget(m_listWidget);

        connect(m_listWidget, &QWidget::customContextMenuRequested, this, &KDFWidget::contextMenuRequested);

        makeColumns();

        mIsTopLevel = QLatin1String(parent->metaObject()->className()) == QLatin1String("KDFTopLevel") ? true : false;
    }

    loadSettings();
    if (init)
        applySettings();
}

KDFWidget::~KDFWidget()
{
    applySettings();

    delete m_listModel;
    delete m_sortModel;
    delete m_itemDelegate;
    delete m_listWidget;
}

void KDFWidget::makeColumns(void)
{
    QStringList columns;
    for (const Column &c : std::as_const(m_columnList)) {
        columns << c.columnName;
    }
    m_listModel->setHorizontalHeaderLabels(columns);
}

/******************************************************************/
void KDFWidget::closeEvent(QCloseEvent *)
{
    qApp->quit();
}

void KDFWidget::settingsChanged(void)
{
    applySettings();
    loadSettings();
}

/***************************************************************************
 * writes the KConfig
 **/
void KDFWidget::applySettings(void)
{
    KConfig m_config;
    KConfigGroup config(&m_config, QLatin1String("KDiskFree"));

    if (GUI) {
        for (const Column &c : std::as_const(m_columnList)) {
            if (!m_listWidget->isColumnHidden(c.number))
                config.writeEntry(c.name, m_listWidget->columnWidth(c.number));
        }

        config.writeEntry("SortColumn", m_sortModel->sortColumn());
        config.writeEntry("SortOrder", (int)m_sortModel->sortOrder());

        // Save the sort order of the QTreeView Header
        QHeaderView *header = m_listWidget->header();
        QList<int> sectionIndices;
        for (int i = 0; i < header->count(); i++) {
            sectionIndices.append(header->visualIndex(i));
        }
        config.writeEntry("HeaderSectionIndices", sectionIndices);
    }
    config.sync();
    updateDF();
}

/***************************************************************************
 * reads the KConfig
 **/
void KDFWidget::loadSettings(void)
{
    mStd.updateConfiguration();

    if (GUI) {
        KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("KDiskFree"));
        for (const Column &c : std::as_const(m_columnList)) {
            int width = config.readEntry(c.name, c.defaultWidth);
            m_listWidget->setColumnWidth(c.number, width);
        }

        KConfigGroup config_visible(KSharedConfig::openConfig(), QStringLiteral("KDFConfig"));
        for (const Column &c : std::as_const(m_columnList)) {
            bool visible = config_visible.readEntry(c.name, true);
            m_listWidget->setColumnHidden(c.number, !visible);
        }

        int sortColumn = config.readEntry("SortColumn", 0);
        int sortOrder = config.readEntry("SortOrder", (int)Qt::AscendingOrder);
        m_listWidget->sortByColumn(sortColumn, Qt::SortOrder(sortOrder));

        // Load the sort order of the QTreeView Header
        // This can also be achieved by header->saveState() and header->restoreState(...),
        // but this would not be "human-readable" any more...
        QHeaderView *header = m_listWidget->header();
        QList<int> sectionIndices;
        sectionIndices = config.readEntry("HeaderSectionIndices", sectionIndices);
        if (sectionIndices.count() == header->count()) {
            for (int i = 0; i < header->count(); i++) {
                int sectionIndex = sectionIndices.at(i);
                int oldVisualIndex = header->visualIndex(sectionIndex);
                header->moveSection(oldVisualIndex, i);
            }
        }

        setUpdateFrequency(mStd.updateFrequency());
        updateDF();
    }
}

/***************************************************************************
 * pops up the SettingsBox if the settingsBtn is clicked
 **/
void KDFWidget::settingsBtnClicked(void)
{
    if (mIsTopLevel) {
        if (mOptionDialog == nullptr) {
            mOptionDialog = new COptionDialog(this);
            if (mOptionDialog == nullptr) {
                return;
            }
            connect(mOptionDialog, &COptionDialog::valueChanged, this, &KDFWidget::settingsChanged);
        }
        mOptionDialog->show();
    }
}

/***************************************************************************
 * resets the timer for automatic df-refreshes
 **/
void KDFWidget::setUpdateFrequency(int frequency)
{
    //
    // Kill current timer and restart it if the frequency is
    // larger than zero.
    //
    QAbstractEventDispatcher::instance()->unregisterTimers(this);
    if (frequency > 0) {
        startTimer(frequency * 1000);
    }
}

/***************************************************************************
 * Update (reread) all disk-dependencies
 **/
void KDFWidget::timerEvent(QTimerEvent *)
{
    updateDF();
}

/***************************************************************************
 * checks fstab & df
 **/
void KDFWidget::updateDF(void)
{
    //
    // We can only do this if the popupmenu is not present
    //
    if (mPopup == nullptr) {
        readingDF = true;
        mDiskList.readFSTAB();
        mDiskList.readDF();
    }
}

/***************************************************************************
 * gets the signal when the diskList is complete and up to date
 **/
void KDFWidget::updateDFDone(void)
{
    if (mPopup) // The popup menu is ont he screen... Don't touch the list view...
        return;

    // Clear the list items
    m_listModel->removeRows(0, m_listModel->rowCount());

    DisksConstIterator itr = mDiskList.disksConstIteratorBegin();
    DisksConstIterator end = mDiskList.disksConstIteratorEnd();
    for (; itr != end; ++itr) {
        DiskEntry *disk = *itr;

        QString size, percent;
        if (disk->kBSize() > 0) {
            percent = i18nc("Disk percentage", "%1%", QString::number(disk->percentFull()));
            size = disk->prettyKBSize();
        } else {
            percent = i18n("N/A");
            size = i18n("N/A");
        }

        bool root = !disk->mountOptions().contains(QLatin1String("user"), Qt::CaseInsensitive);

        QStandardItem *IconItem = new QStandardItem(generateIcon(disk->iconName(), root, disk->mounted()), QLatin1String(""));

        QStandardItem *DeviceItem = new QStandardItem(disk->deviceName());

        QStandardItem *TypeItem = new QStandardItem(disk->fsType());

        QStandardItem *SizeItem = new QStandardItem(size);
        SizeItem->setData(disk->kBSize(), Qt::UserRole);

        QStandardItem *MountPointItem = new QStandardItem(disk->mountPoint());

        QStandardItem *FreeItem = new QStandardItem(disk->prettyKBAvail());
        FreeItem->setData(disk->kBAvail(), Qt::UserRole);

        QStandardItem *FullItem = new QStandardItem(percent);
        FullItem->setData(disk->percentFull(), Qt::UserRole);

        QStandardItem *UsageBarItem = new QStandardItem(QLatin1String(""));
        UsageBarItem->setData(disk->percentFull(), Qt::UserRole);

        m_listModel->appendRow(QList<QStandardItem *>()
                               << IconItem << DeviceItem << TypeItem << SizeItem << MountPointItem << FreeItem << FullItem << UsageBarItem);
    }

    readingDF = false;

    m_listModel->sort(DeviceCol);
}

QIcon KDFWidget::generateIcon(const QString &iconName, bool mode, bool mounted)
{
    const int smallIcon = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);
    QPixmap pix = QIcon::fromTheme(iconName).pixmap(smallIcon);

    QPainter painter(&pix);

    if (mode)
        painter.drawPixmap(QRect(0, 6, 10, 10), QIcon::fromTheme(QStringLiteral("object-locked")).pixmap(smallIcon));

    if (mounted)
        painter.drawPixmap(QRect(6, 6, 12, 12), QIcon::fromTheme(QStringLiteral("emblem-mounted")).pixmap(smallIcon));

    painter.end();
    return QIcon(pix);
}

void KDFWidget::criticallyFull(DiskEntry *disk)
{
    if (mStd.popupIfFull()) {
        QString msg = i18n("Device [%1] on [%2] is critically full.", disk->deviceName(), disk->mountPoint());
        KMessageBox::error(this, msg, i18nc("Warning device getting critically full", "Warning"));
    }
}

DiskEntry *KDFWidget::selectedDisk(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    QStandardItem *itemDevice = m_listModel->item(index.row(), DeviceCol);
    QStandardItem *itemMountPoint = m_listModel->item(index.row(), MountPointCol);

    DiskEntry *disk = new DiskEntry(itemDevice->text());
    disk->setMountPoint(itemMountPoint->text());

    int pos = mDiskList.find(disk);

    delete disk;
    return mDiskList.at(pos);
}

void KDFWidget::contextMenuRequested(const QPoint &p)
{
    if (mPopup) // The user may even be able to popup another menu while this open is active...
        return;

    QModelIndex index = m_listWidget->indexAt(p);

    if (!index.isValid()) {
        QList<QModelIndex> selected = m_listWidget->selectionModel()->selectedIndexes();
        if (selected.size() > 0)
            index = selected.at(0);
        else
            return;
    }

    index = m_sortModel->mapToSource(index);

    mDiskList.setUpdatesDisabled(true);
    DiskEntry *disk = selectedDisk(index);

    if (disk == nullptr)
        return;

    mPopup = new QMenu(nullptr);
    mPopup->setTitle(disk->mountPoint());
    QAction *mountPointAction = mPopup->addAction(QIcon::fromTheme(QStringLiteral("media-mount")), i18n("Mount Device"));
    QAction *umountPointAction = mPopup->addAction(QIcon::fromTheme(QStringLiteral("media-eject")), i18n("Unmount Device"));
    mPopup->addSeparator();
    QAction *openFileManagerAction = mPopup->addAction(QIcon::fromTheme(QStringLiteral("system-file-manager")), i18n("Open in File Manager"));
    mountPointAction->setEnabled(!disk->mounted());
    umountPointAction->setEnabled(disk->mounted());
    openFileManagerAction->setEnabled(disk->mounted());

    QAction *filelightAction = nullptr;
    KService::Ptr filelight = KService::serviceByDesktopName(QStringLiteral("org.kde.filelight"));
    if (filelight) {
        filelightAction = mPopup->addAction(QIcon::fromTheme(filelight->icon()), i18nc("@action:inmenu", "Explore in %1", filelight->name()));
        filelightAction->setEnabled(disk->mounted());
    }

    QAction *position = mPopup->exec(m_listWidget->mapToGlobal(p));

    bool openFileManager = false;
    if (!position) {
        mDiskList.setUpdatesDisabled(false);
        delete mPopup;
        mPopup = nullptr;
        return;
    } else if (position == mountPointAction || position == umountPointAction) {
        QStandardItem *SizeItem = m_listModel->item(index.row(), SizeCol);
        SizeItem->setText(i18n("MOUNTING"));

        QStandardItem *FreeItem = m_listModel->item(index.row(), FreeCol);
        FreeItem->setText(i18n("MOUNTING"));

        QStandardItem *IconItem = m_listModel->item(index.row(), IconCol);
        IconItem->setIcon(QIcon::fromTheme(QStringLiteral("user-away")));

        int val = disk->toggleMount();
        if (val != 0 /*== false*/) {
            KMessageBox::error(this, disk->lastSysError());
        } else if (mStd.openFileManager() && (position == mountPointAction)) // only on mount
        {
            openFileManager = true;
        }

        // delete item;
        mDiskList.deleteAllMountedAt(disk->mountPoint());
    } else if (position == openFileManagerAction) {
        openFileManager = true;
    } else if (position == filelightAction) {
        auto *job = new KIO::ApplicationLauncherJob(filelight);
        job->setUrls({QUrl::fromLocalFile(disk->mountPoint())});
        job->setUiDelegate(new KDialogJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, window()));
        job->start();
    }

    if (openFileManager) {
        qCDebug(KDF) << "opening filemanager";
        if (mStd.useSystemFileManager()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(disk->mountPoint()));
        } else if (!mStd.fileManager().isEmpty()) {
            QString cmd = mStd.fileManager();
            int pos = cmd.indexOf(QLatin1String("%m"));
            if (pos > 0) {
                cmd.replace(pos, 2, KShell::quoteArg(disk->mountPoint()));
            } else {
                cmd += QLatin1Char(' ') + KShell::quoteArg(disk->mountPoint());
            }
            QStringList argList = cmd.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            cmd = argList.takeFirst();
            QProcess::startDetached(cmd, argList);
        }
    }

    // Update only here as showing of error message triggers event loop.
    mDiskList.setUpdatesDisabled(false);
    delete mPopup;
    mPopup = nullptr;

    if (position != openFileManagerAction) // No need to update when just opening the fm.
    {
        updateDF();
    }
}

void KDFWidget::invokeHelp()
{
    KHelpClient::invokeHelp(QLatin1String(""), QStringLiteral("kcontrol/kdf"));
}

#include "moc_kdfwidget.cpp"
