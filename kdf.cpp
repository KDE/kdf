/*
 * kdf.cpp - KDiskFree
 *
 * written 1998-2001 by Michael Kropfberger <michael.kropfberger@gmx.net>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <kapplication.h>
#include <kstdaccel.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kstdaction.h>
#include <kaction.h>

#include "kdf.h"
#include <kpopupmenu.h>

static const char description[] =
	I18N_NOOP("KDE free disk space utility");

static const char version[] = "v0.5";


/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *, const char *name)
  : KMainWindow(0, name)
{
  kdf = new KDFWidget(this,"kdf",FALSE);
  Q_CHECK_PTR(kdf);
  (void) new KAction( i18n( "&Update" ), 0, kdf, SLOT( updateDF() ), actionCollection(), "updatedf" );

  KStdAction::quit(this, SLOT(close()), actionCollection());
  KStdAction::preferences(kdf, SLOT(settingsBtnClicked()), actionCollection());
  KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), 
actionCollection());
  setCentralWidget(kdf);
  //  kdf->setMinimumSize(kdf->sizeHint());
  kdf->resize(kdf->sizeHint());
  setupGUI(KMainWindow::Keys | StatusBar | Save | Create);
}


bool KDFTopLevel::queryExit( void )
{
  kdf->applySettings();
  return( true );
}


/***************************************************************/
int main(int argc, char **argv)
{
  KAboutData aboutData( "kdf", I18N_NOOP("KDiskFree"),
    version, description, KAboutData::License_GPL,
    "(c) 1998-2001, Michael Kropfberger");
  aboutData.addAuthor("Michael Kropfberger",0, "michael.kropfberger@gmx.net");
  KCmdLineArgs::init( argc, argv, &aboutData );

  KApplication app;

  if( app.isRestored() ) //SessionManagement
  {
    for( int n=1; KDFTopLevel::canBeRestored(n); n++ )
    {
      KDFTopLevel *ktl = new KDFTopLevel();
      Q_CHECK_PTR(ktl);
      app.setMainWidget(ktl);
      ktl->restore(n);
    }
  }
  else
  {
    KDFTopLevel *ktl = new KDFTopLevel();
    Q_CHECK_PTR(ktl);
    ktl->show();
  }

  return app.exec();
}

#include "kdf.moc"

