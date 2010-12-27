/*
 * kdf.cpp - KDiskFree
 *
 * Copyright 1998-2001 by Michael Kropfberger <michael.kropfberger@gmx.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "kdf.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kxmlguifactory.h>

#include <kmenu.h>
#include <kdebug.h>
#include <kmenubar.h>
#include <kaction.h>
#include <kstandardshortcut.h>
#include <kstandardaction.h>
#include <kactioncollection.h>

static const char description[] =
    I18N_NOOP("KDE free disk space utility");

static const char version[] = "v0.13"; //bump version ?


/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *)
        : KXmlGuiWindow(0)
{
    kdf = new KDFWidget(this,false);
    Q_CHECK_PTR(kdf);
    QAction *action = actionCollection()->addAction( QLatin1String( "updatedf" ));
    action->setText( i18nc( "Update action", "&Update" ) );
    connect(action, SIGNAL(triggered(bool) ), kdf, SLOT( updateDF() ));

    KStandardAction::quit(this, SLOT(close()), actionCollection());
    KStandardAction::preferences(kdf, SLOT(settingsBtnClicked()), actionCollection());
    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()),
                                 actionCollection());
    setCentralWidget(kdf);
    //  kdf->setMinimumSize(kdf->sizeHint());
    kdf->resize(kdf->sizeHint());
    setupGUI(KXmlGuiWindow::Keys | StatusBar | Save | Create);
}


bool KDFTopLevel::queryExit( void )
{
    kdf->applySettings();
    return( true );
}


/***************************************************************/
int main(int argc, char **argv)
{
    KAboutData aboutData( "kdf", 0, ki18n("KDiskFree"),
                          version, ki18n(description), KAboutData::License_GPL,
                          ki18n("(c) 1998-2001, Michael Kropfberger"), KLocalizedString(),
                          "http://utils.kde.org/projects/kdf");
    aboutData.addAuthor(ki18n("Michael Kropfberger"),KLocalizedString(), "michael.kropfberger@gmx.net");
    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    if( app.isSessionRestored() ) //SessionManagement
    {
        for( int n=1; KDFTopLevel::canBeRestored(n); n++ )
        {
            KDFTopLevel *ktl = new KDFTopLevel();
            Q_CHECK_PTR(ktl);
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

