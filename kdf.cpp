/*
 * $Id$
 *
 * kdf.cpp - KDiskFree
 *
 * written 1998-1999 by Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  
 */

#include <kapp.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstdaccel.h>

#include "kdf.h"

/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *, const char *name)
  : KTMainWindow(name) 
{
  kdf = new KDFWidget(this,"kdf",FALSE); CHECK_PTR(kdf);
  kdf->loadSettings();
  setView(kdf);

  KStdAccel *keys = new KStdAccel();
  if( keys == 0 ) { return; }

  QPopupMenu *file = new QPopupMenu; CHECK_PTR(file);
  file->insertItem( i18n( "&Update" ), kdf, SLOT(updateDF()) );
  file->insertSeparator();
  file->insertItem( i18n( "&Quit" ), kapp, SLOT(quit()), keys->quit() );

  QPopupMenu *option = new QPopupMenu; CHECK_PTR(option);
  option->insertItem( i18n( "&Customize" ), kdf, SLOT(settingsBtnClicked()) );

  QPopupMenu *help = helpMenu(i18n(""
    "KDiskFree\n\n(C) 1998,1999\n"
    "Michael Kropfberger (michael.kropfberger@gmx.net)") );

  menuBar()->insertItem( i18n("&File"), file );
  menuBar()->insertItem( i18n("&Options"), option );
  menuBar()->insertSeparator();
  menuBar()->insertItem( i18n("&Help"), help );

  delete keys;

  resize(kdf->width(),kdf->height()+menuBar()->height());
};


/***************************************************************/
int main(int argc, char **argv)
{
  KApplication app(argc, argv, "kdf");

  if( app.isRestored() ) //SessionManagement
  {
    for( int n=1; KTMainWindow::canBeRestored(n); n++ ) 
    {
      KDFTopLevel *ktl = new KDFTopLevel();
      CHECK_PTR(ktl);
      app.setMainWidget(ktl);
      ktl->restore(n);
    } 
  } 
  else 
  {
    KDFTopLevel *ktl = new KDFTopLevel();
    CHECK_PTR(ktl);
    ktl->show();
  }

  return app.exec();
};

#include "kdf.moc"

