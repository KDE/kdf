/*
    kdf.cpp - KDiskFree

    SPDX-FileCopyrightText: 1998-2001 Michael Kropfberger <michael.kropfberger@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later

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
    KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
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


