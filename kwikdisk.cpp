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

//
// 1999-12-03 Espen Sand
// Cleanups, improvements and fixes for KDE-2
//
// 2004-07-15 Stanislav Karchebny
// Rewrite for KDE 3
//


#include <stdlib.h>
#include <unistd.h>

#include <QtCore/QFile>
#include <QtCore/QAbstractEventDispatcher>

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QActionGroup>

#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <krun.h>
#include <ktoolinvocation.h>
#include <kshell.h>
#include <kdebug.h>

#include "kwikdisk.h"

static const char description[] =
    I18N_NOOP("KDE Free disk space utility");

static const char version[] = "0.4";

/*****************************************************************************/

KwikDisk::KwikDisk()
        : KSystemTrayIcon()
        , m_readingDF(false)
        , m_dirty(true)
        , m_inside(false)
        , m_optionDialog(0)
{
    kDebug() ;

    contextMenu()->setTitle(i18n("KwikDisk"));
    setIcon(KSystemTrayIcon::loadIcon("kdf"));
    show();

    connect( &m_diskList, SIGNAL(readDFDone()), this, SLOT(updateDFDone()) );
    connect( &m_diskList, SIGNAL(criticallyFull(DiskEntry*)),
             this, SLOT(criticallyFull(DiskEntry*)) );

    connect( this, SIGNAL( activated(QSystemTrayIcon::ActivationReason)),
             SLOT(slotActivated(QSystemTrayIcon::ActivationReason) ));

    m_actionGroup = new QActionGroup( this );
    connect( m_actionGroup, SIGNAL( triggered( QAction* ) ) , this, SLOT( toggleMount( QAction* ) ) );

    m_actionSeparator = contextMenu()->addSeparator();

    contextMenu()->addAction(
        KSystemTrayIcon::loadIcon("kdf"),
        i18n("&Start KDiskFree"), this, SLOT(startKDF()));

    contextMenu()->addAction(
        KSystemTrayIcon::loadIcon("configure"),
        i18n("&Configure KwikDisk..."), this, SLOT(changeSettings()));

    contextMenu()->addAction(
        KSystemTrayIcon::loadIcon("help-contents"),
        KStandardGuiItem::help().text(), this, SLOT(invokeHelp()));

    loadSettings();
    updateDF();

}

void KwikDisk::enterEvent(QEvent *)
{
    kDebug() ;
    m_inside = true;
}

void KwikDisk::leaveEvent(QEvent *)
{
    kDebug() ;
    m_inside = false;
}

void KwikDisk::slotActivated(QSystemTrayIcon::ActivationReason)
{
    kDebug() ;

    if( m_dirty )
        updateDF();
}

void KwikDisk::loadSettings()
{
    kDebug() ;

    m_options.updateConfiguration();
    setUpdateFrequency( m_options.updateFrequency() );
}

void KwikDisk::setUpdateFrequency(int frequency)
{
    kDebug() ;

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
    kDebug() ;
    m_dirty = true;
}

void KwikDisk::updateDF()
{
    kDebug() ;

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
    kDebug() ;

    m_readingDF = false;
    m_dirty     = false;

    clearDeviceActions();

    int itemNo = 0;
    
    DisksConstIterator itr = m_diskList.disksConstIteratorBegin();
    DisksConstIterator end = m_diskList.disksConstIteratorEnd();
    for (; itr != end; ++itr)
    {
        DiskEntry * disk = *itr;

        QString toolTipText = i18n("%1 (%2) %3 on %4",
                                   disk->mounted() ? i18nc("Unmount the storage device", "Unmount") : i18nc("Mount the storage device", "Mount"),
                                   disk->fsType().trimmed(), disk->deviceName().trimmed(), disk->mountPoint().trimmed());

        QString entryName = disk->mountPoint().trimmed();
        if( disk->mounted() )
        {
            entryName += QString("\t[%1]").arg(disk->prettyKBAvail());
        }

        QAction * action = new QAction(entryName, m_actionGroup);
        action->setToolTip( toolTipText );
        action->setData( itemNo );
        itemNo++;

        QPixmap pix = KSystemTrayIcon::loadIcon(disk->iconName()).pixmap( QSize(32,32) );

        if( getuid() !=0 && !disk->mountOptions().contains("user",Qt::CaseInsensitive) )
        {
            QPainter painter( &pix );
            painter.drawPixmap( QRect(0,0,16,16) , SmallIcon("object-locked"), QRect(0,0,16,16));
            painter.end();

            toolTipText = i18n("You must login as root to mount this disk");
            action->setToolTip( toolTipText );
        }

        if( disk->mounted() )
        {
            QPainter painter ( &pix );
            painter.drawPixmap( QRect(8,8,16,16) , SmallIcon("emblem-mounted"), QRect(0,0,16,16) );
            painter.end();
        }

        action->setIcon( pix );

    }

    contextMenu()->insertActions( m_actionSeparator, m_actionGroup->actions() );

}

void KwikDisk::toggleMount(QAction * action)
{
    kDebug() ;
    if ( !action )
        return;

    DiskEntry *disk = m_diskList.at( action->data().toInt() );
    if( disk == 0 )
    {
        return;
    }

    int val = disk->toggleMount();
    if( val != 0 )
    {
        KMessageBox::error(0, disk->lastSysError());
    }
    else if( (m_options.openFileManager() == true) && (disk->mounted() == true ) )
    {
        kDebug() << "opening filemanager" ;
        if( m_options.fileManager().isEmpty() == false )
        {
            QString cmd = m_options.fileManager();
            int pos = cmd.indexOf("%m");
            if( pos > 0 )
            {
                cmd = cmd.replace( pos, 2, KShell::quoteArg(disk->mountPoint()) ) + " &";
            }
            else
            {
                cmd += ' ' + KShell::quoteArg(disk->mountPoint()) +" &";
            }
            system( QFile::encodeName(cmd) );
        }
    }
    m_dirty = true;
}

void KwikDisk::criticallyFull(DiskEntry *disk)
{
    kDebug() ;

    if( m_options.popupIfFull() == true )
    {
        QString msg = i18n("Device [%1] on [%2] is critically full.",
                           disk->deviceName(), disk->mountPoint());
        KMessageBox::sorry( 0, msg, i18nc("Device is getting critically full", "Warning"));
    }
}

void KwikDisk::changeSettings()
{
    if( m_optionDialog == 0 )
    {
        m_optionDialog = new COptionDialog(0);
        if( !m_optionDialog )
            return;
        connect(m_optionDialog, SIGNAL(valueChanged()),
                this, SLOT(loadSettings()));
    }
    m_optionDialog->show();
}

void KwikDisk::startKDF()
{
    kDebug() ;

    KRun::runCommand("kdf",NULL);
}

void KwikDisk::invokeHelp()
{
    KToolInvocation::invokeHelp("", "kdf");
}

/*****************************************************************************/

int main(int argc, char **argv)
{
    KLocale::setMainCatalog( "kdf" );

    KAboutData about("kwikdisk", 0, ki18n("KwikDisk"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2004 Stanislav Karchebny"),
                     KLocalizedString(),  "http://utils.kde.org/projects/kdf",
                     "Stanislav.Karchebny@kdemail.net");
    about.addAuthor( ki18n("Michael Kropfberger"), ki18n("Original author"),
                     "michael.kropfberger@gmx.net" );
    about.addAuthor( ki18n("Espen Sand"), ki18n("KDE 2 changes"));
    about.addAuthor( ki18n("Stanislav Karchebny"), ki18n("KDE 3 changes"),
                     "Stanislav.Karchebny@kdemail.net" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;

    KwikDisk *mainWin = 0;
    mainWin = new KwikDisk();

    //Avoid quiting when closing the KwikDisk Settings dialog
    app.setQuitOnLastWindowClosed( false );

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

/*****************************************************************************/

#include "kwikdisk.moc"

