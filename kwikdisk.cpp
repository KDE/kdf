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

//
// 1999-12-03 Espen Sand
// Cleanups, improvements and fixes for KDE-2
//
// 2004-07-15 Stanislav Karchebny
// Rewrite for KDE 3
//

#include "kwikdisk.h"

#include <stdlib.h>

#include <qpen.h>
#include <qbitmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <QTimerEvent>
#include <QEvent>
#include <QAbstractEventDispatcher>
#include <kmainwindow.h>
#include <klocale.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <krun.h>
#include <ktoolinvocation.h>

static const char description[] =
   I18N_NOOP("KDE Free disk space utility");

static const char version[] = "0.2";

static KCmdLineOptions options[] =
{
   KCmdLineLastOption
};

/*****************************************************************************/

KwikDisk::KwikDisk()
   : KSystemTray()
   , m_readingDF(false)
   , m_dirty(true)
   , m_menuVisible(false)
   , m_inside(false)
   , m_optionDialog(0)
{
   kDebug() << k_funcinfo << endl;

   setPixmap(KSystemTray::loadIcon("kdf"));
   show();

   connect( &m_diskList, SIGNAL(readDFDone()), this, SLOT(updateDFDone()) );
   connect( &m_diskList, SIGNAL(criticallyFull(DiskEntry*)),
            this, SLOT(criticallyFull(DiskEntry*)) );

   connect( contextMenu(), SIGNAL(aboutToHide()), this, SLOT(aboutToHide()) );

   loadSettings();
   updateDF();
}

void KwikDisk::aboutToHide()
{
   kDebug() << k_funcinfo << endl;
   if( !m_inside )
      m_menuVisible = false;
}

void KwikDisk::enterEvent(QEvent *)
{
   kDebug() << k_funcinfo << endl;
   m_inside = true;
}

void KwikDisk::leaveEvent(QEvent *)
{
   kDebug() << k_funcinfo << endl;
   m_inside = false;
}

void KwikDisk::mousePressEvent(QMouseEvent *me)
{
   kDebug() << k_funcinfo << endl;

   if( m_dirty )
      updateDF();

   if( m_menuVisible )
   {
      contextMenu()->hide();
      m_menuVisible = false;
      me->ignore();
      return;
   }

   contextMenuAboutToShow(contextMenu());
   contextMenu()->popup( me->globalPos() );
   m_menuVisible = true;
}

void KwikDisk::loadSettings()
{
   kDebug() << k_funcinfo << endl;

   m_options.updateConfiguration();
   setUpdateFrequency( m_options.updateFrequency() );
}

void KwikDisk::setUpdateFrequency(int frequency)
{
   kDebug() << k_funcinfo << endl;

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
   kDebug() << k_funcinfo << endl;
   m_dirty = true;
}

void KwikDisk::updateDF()
{
   kDebug() << k_funcinfo << endl;

   m_readingDF = true;
   m_diskList.readFSTAB();
   m_diskList.readDF();
}

void KwikDisk::updateDFDone()
{
   kDebug() << k_funcinfo << endl;

   m_readingDF = false;
   m_dirty     = false;

   contextMenu()->clear();
   contextMenu()->addTitle(KSystemTray::loadIcon("kdf"), i18n("KwikDisk"));

   int itemNo = 0;
   for( DiskEntry *disk = m_diskList.first(); disk != 0; disk = m_diskList.next() )
   {
      // FIXME: tool tips are unused atm
      QString toolTipText = i18n("%1 (%2) %3 on %4",
           disk->mounted() ? i18n("Unmount") : i18n("Mount"),
          disk->fsType(), disk->deviceName(), disk->mountPoint());

      QString entryName = disk->mountPoint();
      if( disk->mounted() )
      {
         entryName += QString("\t\t\t[%1]").arg(disk->prettyKBAvail());
      }
      int id = contextMenu()->insertItem("", this, SLOT(toggleMount(int)) );
      contextMenu()->setItemParameter(id, itemNo);
      itemNo++;

      QPixmap *pix = new QPixmap(KSystemTray::loadIcon(disk->iconName()));

      if( getuid() !=0 && !disk->mountOptions().contains("user",Qt::CaseInsensitive) == -1 )
      {
         //
         // Special root icon, normal user cant mount.
         //
         // 2000-01-23 Espen Sand
         // Careful here: If the mask has not been defined we can
         // not use QPixmap::mask() because it returns 0 => segfault
         //
         if( !pix->mask().isNull() )
         {
            QBitmap *bm = new QBitmap((pix->mask()));
            if( bm != 0 )
            {
               QPainter qp( bm );
               qp.setPen(QPen(Qt::white,1));
               qp.drawRect(0,0,bm->width(),bm->height());
               qp.end();
               pix->setMask(*bm);
            }
            QPainter qp( pix );
            qp.setPen(QPen(Qt::red,1));
            qp.drawRect(0,0,pix->width(),pix->height());
            qp.end();
         }
         contextMenu()->disconnectItem(id,disk,SLOT(toggleMount()));
         toolTipText = i18n("You must login as root to mount this disk");
      }

      contextMenu()->changeItem(id,*pix,entryName);
   }

   contextMenu()->insertSeparator();

   contextMenu()->insertItem(
      KSystemTray::loadIcon("kdf"),
      i18n("&Start KDiskFree"), this, SLOT(startKDF()),0);

   contextMenu()->insertItem(
      KSystemTray::loadIcon("configure"),
      i18n("&Configure KwikDisk..."), this, SLOT(changeSettings()),0);

   contextMenu()->insertItem(
      KSystemTray::loadIcon("help"),
      KStdGuiItem::help().text(), this, SLOT(invokeHelp()),0);

   contextMenu()->insertSeparator();

   contextMenu()->insertItem(
      KSystemTray::loadIcon("exit"),
      KStdGuiItem::quit().text(), this, SIGNAL(quitSelected()) );
}

void KwikDisk::toggleMount(int item)
{
   kDebug() << k_funcinfo << endl;

   DiskEntry *disk = m_diskList.at(item);
   if( disk == 0 )
   {
      return;
   }

   int val = disk->toggleMount();
   if( val != 0 )
   {
      KMessageBox::error(this, disk->lastSysError());
   }
   else if( (m_options.openFileManager() == true) && (disk->mounted() == true ) )
   {
      kDebug() << "opening filemanager" << endl;
      if( m_options.fileManager().isEmpty() == false )
      {
         QString cmd = m_options.fileManager();
         int pos = cmd.indexOf("%m");
         if( pos > 0 )
         {
            cmd = cmd.replace( pos, 2, KProcess::quote(disk->mountPoint()) ) + " &";
         }
         else
         {
            cmd += " " + KProcess::quote(disk->mountPoint()) +" &";
         }
         system( QFile::encodeName(cmd) );
      }
   }
   m_dirty = true;
}

void KwikDisk::criticallyFull(DiskEntry *disk)
{
   kDebug() << k_funcinfo << endl;

   if( m_options.popupIfFull() == true )
   {
      QString msg = i18n("Device [%1] on [%2] is critically full.",
                     disk->deviceName(), disk->mountPoint());
      KMessageBox::sorry( this, msg, i18n("Warning"));
   }
}

void KwikDisk::changeSettings()
{
   if( m_optionDialog == 0 )
   {
      m_optionDialog = new COptionDialog(this, "options", false);
      if( !m_optionDialog ) return;
      connect(m_optionDialog, SIGNAL(valueChanged()),
                        this, SLOT(loadSettings()));
   }
   m_optionDialog->show();
}

void KwikDisk::startKDF()
{
   kDebug() << k_funcinfo << endl;

   KRun::runCommand("kdf");
}

void KwikDisk::invokeHelp()
{
   KToolInvocation::invokeHelp("", "kdf");
}

/*****************************************************************************/

int main(int argc, char **argv)
{
   KLocale::setMainCatalog( "kdf" );

   KAboutData about("kwikdisk", I18N_NOOP("KwikDisk"), version, description,
                  KAboutData::License_GPL, "(C) 2004 Stanislav Karchebny",
                  0, 0, "Stanislav.Karchebny@kdemail.net");
   about.addAuthor( "Michael Kropfberger", I18N_NOOP("Original author"),
                    "michael.kropfberger@gmx.net" );
   about.addAuthor( "Espen Sand", I18N_NOOP("KDE 2 changes"), "" );
   about.addAuthor( "Stanislav Karchebny", I18N_NOOP("KDE 3 changes"),
                    "Stanislav.Karchebny@kdemail.net" );
   KCmdLineArgs::init(argc, argv, &about);
   KCmdLineArgs::addCmdLineOptions( options );
   KApplication app;
   KwikDisk *mainWin = 0;

   mainWin = new KwikDisk;
   QObject::connect(mainWin, SIGNAL(quitSelected()), &app, SLOT(quit()));

   // mainWin has WDestructiveClose flag by default, so it will delete itself.
   return app.exec();
}

/*****************************************************************************/

#include "kwikdisk.moc"
