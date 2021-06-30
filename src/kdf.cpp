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

#include "kdf_version.h"
#include <kxmlgui_version.h>
#include <KAboutData>
#include <KXMLGUIFactory>
#include <KStandardShortcut>
#include <KStandardAction>
#include <KActionCollection>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

static const char description[] =
    I18N_NOOP("KDE free disk space utility");

/***************************************************************/
KDFTopLevel::KDFTopLevel(QWidget *)
        : KXmlGuiWindow(nullptr)
{
    kdf = new KDFWidget(this,false);
    Q_CHECK_PTR(kdf);
    QAction *action = actionCollection()->addAction( QStringLiteral("updatedf"));
    action->setText( i18nc( "Update action", "&Update" ) );
    actionCollection()->setDefaultShortcuts(action, KStandardShortcut::reload());
    connect(action, &QAction::triggered, kdf, &KDFWidget::updateDF);

    KStandardAction::quit(this, &KDFTopLevel::close, actionCollection());
    KStandardAction::preferences(kdf, &KDFWidget::settingsBtnClicked, actionCollection());
#if KXMLGUI_VERSION >= QT_VERSION_CHECK(5, 84, 0)
    KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
#else
    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()),
                                 actionCollection());
#endif
    setCentralWidget(kdf);
    //  kdf->setMinimumSize(kdf->sizeHint());
    kdf->resize(kdf->sizeHint());
    setupGUI(KXmlGuiWindow::Keys | StatusBar | Save | Create);
}


void KDFTopLevel::closeEvent(QCloseEvent *event)
{
    kdf->applySettings();
    KXmlGuiWindow::closeEvent(event);
}


/***************************************************************/
int main(int argc, char **argv)
{
    //Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kdf");

    KAboutData aboutData(QStringLiteral("kdf"),
                         i18n("KDiskFree"),
                         QStringLiteral(KDF_VERSION_STRING),
                         i18n(description),
                         KAboutLicense::GPL,
                         i18n("(c) 1998-2001, Michael Kropfberger"),
                         QString(),
                         QStringLiteral("http://utils.kde.org/projects/kdf"),
                         QString()
                        );

    aboutData.setOrganizationDomain("kde.org");
    aboutData.addAuthor(i18n("Michael Kropfberger"),
                        QString(),
                        QStringLiteral("michael.kropfberger@gmx.net"));

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    aboutData.setupCommandLine(&parser);

    KAboutData::setApplicationData(aboutData);

    // do the command line parsing
    parser.process(app);

    // handle standard options
    aboutData.processCommandLine(&parser);

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


