/*
 * $Id$
 *
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <kapp.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstdaccel.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kdf.h"

static const char *description =
	I18N_NOOP("KDE free disk space utility");

static const char *version = "v0.5";


/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *, const char *name)
  : KMainWindow(0, name)
{
  kdf = new KDFWidget(this,"kdf",FALSE); Q_CHECK_PTR(kdf);

  QPopupMenu *file = new QPopupMenu; Q_CHECK_PTR(file);
  file->insertItem( i18n( "&Update" ), kdf, SLOT(updateDF()) );
  file->insertSeparator();
  file->insertItem( i18n( "&Quit" ), this, SLOT(close()), KStdAccel::key(KStdAccel::Quit) );

  QPopupMenu *option = new QPopupMenu; Q_CHECK_PTR(option);
  option->insertItem( i18n("&Configure %1...").arg(kapp->caption()),
		      kdf, SLOT(settingsBtnClicked()) );

  QPopupMenu * help = helpMenu();
  menuBar()->insertItem( i18n("&File"), file );
  menuBar()->insertItem( i18n("&Options"), option );
  menuBar()->insertSeparator();
  menuBar()->insertItem( i18n("&Help"), help );

  setCentralWidget(kdf);
  //  kdf->setMinimumSize(kdf->sizeHint());
  kdf->resize(kdf->sizeHint());
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
};

#include "kdf.moc"

