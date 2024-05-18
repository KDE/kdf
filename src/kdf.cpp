/*
    SPDX-FileCopyrightText: 1998-2001 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kdf.h"

#include "kdf_version.h"
#include <KAboutData>
#include <KXMLGUIFactory>
#include <KStandardShortcut>
#include <KStandardAction>
#include <KActionCollection>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>

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
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kdf"));

    KAboutData aboutData(QStringLiteral("kdf"),
                         i18n("KDiskFree"),
                         QStringLiteral(KDF_VERSION_STRING),
                         i18n("KDE free disk space utility"),
                         KAboutLicense::GPL,
                         i18n("(c) 1998-2001, Michael Kropfberger"),
                         QString(),
                         QStringLiteral("https://apps.kde.org/kdf"),
                         QString()
                        );

    aboutData.addAuthor(i18nc("@info:credit", "Michael Kropfberger"),
                        QString(),
                        QStringLiteral("michael.kropfberger@gmx.net"));

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    aboutData.setupCommandLine(&parser);

    KAboutData::setApplicationData(aboutData);
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kdf")));

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



#include "moc_kdf.cpp"
