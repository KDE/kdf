/*
  kdf.cpp - KDiskFree

  written 1998-1999 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */


#include <kapp.h>
#include <kwm.h>
#include <kiconloader.h>
#include <qlabel.h>
#include "kdf.h"
#include "kdf.moc"


/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *, const char *name=0)
      : KTopLevelWidget(name) 
{
   KMenuBar *menu = new KMenuBar(this);
   kdf = new KDFWidget(this,"kdf",FALSE); CHECK_PTR(kdf);
   kdf->loadSettings();

   QPopupMenu *file = new QPopupMenu; CHECK_PTR(file);
   file->insertItem( klocale->translate( "&Update" )
                        , kdf, SLOT(updateDF()) );
   file->insertItem( klocale->translate( "&Settings" )
                        , kdf, SLOT(settingsBtnClicked()) );
   file->insertSeparator();
   file->insertItem( klocale->translate( "&Quit" ), kapp, SLOT(quit()) );
   menu->insertItem( klocale->translate( "&File" ), file );

   QPopupMenu *help = kapp->getHelpMenu(TRUE,klocale->translate(
        "KDiskFree\n\n (C) 1998,1999 by Michael Kropfberger (michael.kropfberger@gmx.net)") );
   menu->insertItem( klocale->translate( "&Help" ), help );  

   menu->show();
   setMenu(menu);
   this->setMinimumSize(440,180);
   resize(kdf->width(),kdf->height()+menu->height());
   setView(kdf);
};


/***************************************************************/
int main(int argc, char **argv)
{
  KApplication app(argc, argv,"kdf");
  //SessionManagement
    if (app.isRestored()) {
      int n = 1;
      while (KTopLevelWidget::canBeRestored(n)) {
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
