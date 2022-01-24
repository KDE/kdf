/*
    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

//
// 1999-12-03 Espen Sand
// Cleanups, improvements and fixes for KDE-2
//
// 2004-07-15 Stanislav Karchebny
// Rewrite for KDE 3
//

#include "kwikdisk.h"

#include "kdf_version.h"
#include "kwikdisk_debug.h"

#include <KAboutData>
#include <KDialogJobUiDelegate>
#include <KHelpClient>
#include <KMessageBox>
#include <KIO/CommandLauncherJob>
#include <KShell>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QAbstractEventDispatcher>
#include <QPainter>
#include <QPixmap>
#include <QActionGroup>
#include <QMenu>
#include <QProcess>

#include <unistd.h>

/*****************************************************************************/

KwikDisk::KwikDisk()
        : KStatusNotifierItem()
        , m_readingDF(false)
        , m_dirty(true)
        , m_inside(false)
        , m_optionDialog(nullptr)
{
    qCDebug(KDF);

    setIconByName(QStringLiteral("kdf"));

    connect( &m_diskList, &DiskList::readDFDone, this, &KwikDisk::updateDFDone );
    connect( &m_diskList, &DiskList::criticallyFull,
             this, &KwikDisk::criticallyFull );

    m_actionGroup = new QActionGroup( this );
    connect( m_actionGroup, &QActionGroup::triggered , this, &KwikDisk::toggleMount );

    QMenu *contextMenu = new QMenu(i18n("KwikDisk"));

    m_actionSeparator = contextMenu->addSeparator();

    contextMenu->addAction(
        QIcon::fromTheme(QStringLiteral( "kdf" )),
        i18n("&Start KDiskFree"), this, &KwikDisk::startKDF);

    contextMenu->addAction(
        QIcon::fromTheme(QStringLiteral( "configure" )),
        i18n("&Configure KwikDisk..."), this, &KwikDisk::changeSettings);

    contextMenu->addAction(
        QIcon::fromTheme(QStringLiteral( "help-contents" )),
        KStandardGuiItem::help().text(), this, &KwikDisk::invokeHelp);

    setContextMenu(contextMenu);

    setStatus(KStatusNotifierItem::Active);

    loadSettings();
    updateDF();
}

void KwikDisk::enterEvent(QEvent *)
{
    qCDebug(KDF);
    m_inside = true;
}

void KwikDisk::leaveEvent(QEvent *)
{
    qCDebug(KDF);
    m_inside = false;
}

void KwikDisk::activate(const QPoint &pos)
{
    Q_UNUSED(pos);
    qCDebug(KDF);

    if( m_dirty )
        updateDF();
}

void KwikDisk::loadSettings()
{
    qCDebug(KDF);

    m_options.updateConfiguration();
    setUpdateFrequency( m_options.updateFrequency() );
}

void KwikDisk::setUpdateFrequency(int frequency)
{
    qCDebug(KDF);

    //
    // Kill current timer and restart it if the frequency is
    // larger than zero.
    //
    QAbstractEventDispatcher::instance()->unregisterTimers(this);
    if( frequency > 0 )
    {
        startTimer(frequency * 1000);
    }
}

/**
 * Mark the list as dirty thus forcing a reload the next time the
 * popup menu is about to become visible. Note: A current visible popup
 * will not be updated now.
 */
void KwikDisk::timerEvent(QTimerEvent *)
{
    qCDebug(KDF);
    m_dirty = true;
}

void KwikDisk::updateDF()
{
    qCDebug(KDF);

    m_readingDF = true;
    m_diskList.readFSTAB();
    m_diskList.readDF();
}

void KwikDisk::clearDeviceActions()
{
    QList<QAction*> listActions = m_actionGroup->actions();
    QMutableListIterator<QAction*> it(listActions);
    while( it.hasNext() )
    {
        QAction * action = it.next();
        m_actionGroup->removeAction( action );
        contextMenu()->removeAction( action );
        delete action;
    }
}
void KwikDisk::updateDFDone()
{
    qCDebug(KDF);

    m_readingDF = false;
    m_dirty     = false;

    clearDeviceActions();

    int itemNo = 0;

    DisksConstIterator itr = m_diskList.disksConstIteratorBegin();
    DisksConstIterator end = m_diskList.disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;

        QString toolTipText = QStringLiteral("%1 (%2) %3 on %4")
          .arg( disk->mounted() ? i18nc("Unmount the storage device", "Unmount") : i18nc("Mount the storage device", "Mount") )
          .arg( disk->fsType().trimmed() ).arg( disk->deviceName().trimmed() ).arg( disk->mountPoint().trimmed() );

        QString entryName = disk->mountPoint().trimmed();
        if( disk->mounted() )
        {
            entryName += QStringLiteral("\t[%1]").arg(disk->prettyKBAvail());
        }

        QAction * action = new QAction(entryName, m_actionGroup);
        action->setToolTip( toolTipText );
        action->setData( itemNo );
        itemNo++;

        QPixmap pix = QIcon::fromTheme(disk->iconName()).pixmap(QSize(32,32));

        if( getuid() !=0 && !disk->mountOptions().contains(QLatin1String( "user" ),Qt::CaseInsensitive) )
        {
            QPainter painter( &pix );
            QPixmap lockedPixmap = QIcon::fromTheme(QStringLiteral( "object-locked" )).pixmap(QSize(16,16));
            painter.drawPixmap( QRect(0,0,16,16) , lockedPixmap, QRect(0,0,16,16));
            painter.end();

            toolTipText = i18n("You must login as root to mount this disk");
            action->setToolTip( toolTipText );
        }

        if( disk->mounted() )
        {
            QPainter painter ( &pix );
            QPixmap mountedPixmap = QIcon::fromTheme(QStringLiteral( "emblem-mounted" )).pixmap(QSize(16,16));
            painter.drawPixmap( QRect(8,8,16,16) , mountedPixmap, QRect(0,0,16,16) );
            painter.end();
        }

        action->setIcon( pix );

    }

    contextMenu()->insertActions( m_actionSeparator, m_actionGroup->actions() );

}

void KwikDisk::toggleMount(QAction * action)
{
    qCDebug(KDF);
    if ( !action )
        return;

    DiskEntry *disk = m_diskList.at( action->data().toInt() );
    if( disk == nullptr )
    {
        return;
    }

    int val = disk->toggleMount();
    if( val != 0 )
    {
        KMessageBox::error(nullptr, disk->lastSysError());
    }
    else if( m_options.openFileManager() && ( disk->mounted() ) )
    {
        qCDebug(KDF) << "opening filemanager";
        if( !m_options.fileManager().isEmpty() )
        {
            QString cmd = m_options.fileManager();
            int pos = cmd.indexOf(QLatin1String( "%m" ));
            if( pos > 0 )
            {
                cmd.replace( pos, 2, KShell::quoteArg(disk->mountPoint()) );
            }
            else
            {
                cmd += QLatin1Char( ' ' ) + KShell::quoteArg(disk->mountPoint());
            }
            QStringList argList = cmd.split(QLatin1Char( ' ' ), Qt::SkipEmptyParts);
            cmd = argList.takeFirst();
            QProcess::startDetached(cmd, argList);
        }
    }
    m_dirty = true;
}

void KwikDisk::criticallyFull(DiskEntry *disk)
{
    qCDebug(KDF);

    if( m_options.popupIfFull())
    {
        QString msg = i18n("Device [%1] on [%2] is critically full.",
                           disk->deviceName(), disk->mountPoint());
        KMessageBox::sorry( nullptr, msg, i18nc("Device is getting critically full", "Warning"));
    }
}

void KwikDisk::changeSettings()
{
    if( m_optionDialog == nullptr )
    {
        m_optionDialog = new COptionDialog(nullptr);
        if( !m_optionDialog )
            return;
        connect(m_optionDialog, &COptionDialog::valueChanged,
                this, &KwikDisk::loadSettings);
    }
    m_optionDialog->show();
}

void KwikDisk::startKDF()
{
    qCDebug(KDF);

    auto *job = new KIO::CommandLauncherJob(QStringLiteral("kdf"));
    job->setUiDelegate(new KDialogJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
    job->start();
}

void KwikDisk::invokeHelp()
{
    KHelpClient::invokeHelp(QLatin1String(""), QStringLiteral("kdf"));
}

/*****************************************************************************/

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kdf");

    KAboutData aboutData(QStringLiteral("kwikdisk"),
                     i18n("KwikDisk"),
                     QStringLiteral(KDF_VERSION_STRING),
                     i18n("KDE Free disk space utility"),
                     KAboutLicense::GPL,
                     i18n("(C) 2004 Stanislav Karchebny"),
                     QString(),
                     QStringLiteral("http://utils.kde.org/projects/kdf"),
                     QStringLiteral("Stanislav.Karchebny@kdemail.net"));

    aboutData.addAuthor(i18n("Michael Kropfberger"),
                    i18n("Original author"),
                    QStringLiteral("michael.kropfberger@gmx.net"));
    aboutData.addAuthor(i18n("Espen Sand"),
                    i18n("KDE 2 changes"));
    aboutData.addAuthor(i18n("Stanislav Karchebny"),
                    i18n("KDE 3 changes"),
                    QStringLiteral("Stanislav.Karchebny@kdemail.net"));

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    aboutData.setupCommandLine(&parser);

    KAboutData::setApplicationData(aboutData);

    // do the command line parsing
    parser.process(app);

    // handle standard options
    aboutData.processCommandLine(&parser);

    KwikDisk mainWin;

    //Avoid quit when closing the KwikDisk Settings dialog
    app.setQuitOnLastWindowClosed( false );

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

/*****************************************************************************/


