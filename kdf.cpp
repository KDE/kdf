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

#include <qlabel.h>

#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <kwm.h>
#include <kiconloader.h>

#include "kdf.h"

/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *, const char *name)
      : KTMainWindow(name) 
{
   KMenuBar *menu = new KMenuBar(this);
   kdf = new KDFWidget(this,"kdf",FALSE); CHECK_PTR(kdf);
   kdf->loadSettings();

   QPopupMenu *file = new QPopupMenu; CHECK_PTR(file);
   file->insertItem( i18n( "&Update" )
                        , kdf, SLOT(updateDF()) );
   file->insertItem( i18n( "&Settings" )
                        , kdf, SLOT(settingsBtnClicked()) );
   file->insertSeparator();
   file->insertItem( i18n( "&Quit" ), kapp, SLOT(quit()) );
   menu->insertItem( i18n( "&File" ), file );

   QPopupMenu *help = kapp->helpMenu(TRUE, i18n(
        "KDiskFree\n\n (C) 1998,1999 by Michael Kropfberger (michael.kropfberger@gmx.net)") );
   menu->insertItem( i18n( "&Help" ), help );  

   menu->show();
   setMenu(menu);
   this->setMinimumSize(440,180);
   resize(kdf->width(),kdf->height()+menu->height());
   setView(kdf);
};


/***************************************************************/
int main(int argc, char **argv)
{
  KApplication app(argc, argv, "kdf");
  //SessionManagement
    if (app.isRestored()) {
      int n = 1;
      while (KTMainWindow::canBeRestored(n)) {
        KDFTopLevel *ktl = new KDFTopLevel();
        CHECK_PTR(ktl);
        ktl->setCaption("KDiskFree");
        app.setMainWidget(ktl);
        ktl->restore(n);
        n++;
      } 
    } else {
        KDFTopLevel *ktl = new KDFTopLevel();
        CHECK_PTR(ktl);
        ktl->setCaption("KDiskFree");
        app.setMainWidget(ktl);
        ktl->show();
    }

  return app.exec();
};

#include "kdf.moc"
