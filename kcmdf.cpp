/*
  kcmdf.cpp - KcmDiskFree

  written 1998 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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


#include <stdio.h>

#include "kcmdf.h"

#define KDFTITLE i18n("&KDiskFree")
#define KCWTITLE i18n("&GeneralSettings")
#define MCWTITLE i18n("(U)&MountCommands")

KDiskFree::KDiskFree(int &argc, char **argv, const char *name)
  : KControlApplication(argc, argv, name)
{
  kdf = 0;
  mcw = 0;
  kcw = 0;

  if (runGUI())
    {
      if (!pages || pages->contains("kdf"))
	addPage(kdf = new KDFWidget(dialog, "kdf", FALSE), 
	     KDFTITLE, "index.html");
      if (!pages || pages->contains("kcw"))
	addPage(kcw = new KDFConfigWidget(dialog, "kcw", FALSE), 
	     KCWTITLE, "index.html");
      if (!pages || pages->contains("mcw"))
	addPage(mcw = new MntConfigWidget(dialog, "mcw", FALSE), 
	     MCWTITLE, "index.html");

      //dialog->setApplyButton(0);
      //dialog->setCancelButton(0);
      if (kdf) {
	connect(this->getDialog(),SIGNAL(selected(const char *)),
	    this,SLOT(selected(const char *)) );
        dialog->show();
      } else {
          fprintf(stderr, i18n("usage: kcmdf [-init|kdf]\n"));
	  justInit = TRUE;
      }
    }
}

void KDiskFree::init()
{
  KDFWidget *kdfconfig = new KDFWidget(0, 0, TRUE);
  delete kdfconfig;
  MntConfigWidget *mcwconfig = new MntConfigWidget(0, 0, TRUE);
  delete mcwconfig;
  KDFConfigWidget *kcwconfig = new KDFConfigWidget(0, 0, TRUE);
  delete kcwconfig;
}

void KDiskFree::apply()
{  
  if ((kdf) && (selectedTab==KDFTITLE))
      kdf->applySettings();      
  if ((mcw) && (selectedTab==MCWTITLE))
      mcw->applySettings();      
  if ((kcw) && (selectedTab==KCWTITLE))
      kcw->applySettings();      
  if (kdf) kdf->loadSettings(); // get new changes
}

void KDiskFree::defaultValues()
{  
  if ((kdf) && (selectedTab==KDFTITLE))
      kdf->loadSettings();      
  if ((mcw) && (selectedTab==MCWTITLE))
      mcw->loadSettings();      
  if ((kcw) && (selectedTab==KCWTITLE))
      kcw->loadSettings();      
}

void KDiskFree::selected(const char * tab) {
  selectedTab=tab;
}


int main(int argc, char **argv)
{
  KDiskFree app(argc, argv,"kdf");
  app.setTitle(i18n("KDiskFree"));
  if (app.runGUI()) {
     return app.exec();
  } else {
      app.init();
      return 0;
  }
}

#include "kcmdf.moc"

