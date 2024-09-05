/*
    SPDX-FileCopyrightText: 1999 Espen Sand <espen@kde.org>
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "optiondialog.h"

#include "kdfconfig.h"
#include "mntconfig.h"

#include <KHelpClient>

COptionDialog::COptionDialog(QWidget *parent)
    : KPageDialog(parent)
{
    setWindowTitle(i18n("Configure"));
    setStandardButtons(QDialogButtonBox::Help | QDialogButtonBox::Apply | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    setFaceType(KPageDialog::Tabbed);

    mConf = new KDFConfigWidget(this);
    connect(mConf, &KDFConfigWidget::configChanged, this, &COptionDialog::slotChanged);
    addPage(mConf, i18n("General Settings"));

    mMnt = new MntConfigWidget(this);
    connect(mMnt, &MntConfigWidget::configChanged, this, &COptionDialog::slotChanged);
    addPage(mMnt, i18n("Mount Commands"));

    button(QDialogButtonBox::Apply)->setEnabled(false);
    dataChanged = false;
    connect(button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &COptionDialog::slotOk);
    connect(button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &COptionDialog::slotApply);
    connect(button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &COptionDialog::slotHelp);
}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::slotOk()
{
    if (dataChanged)
        slotApply();
    accept();
}

void COptionDialog::slotApply()
{
    mConf->applySettings();
    mMnt->applySettings();
    Q_EMIT valueChanged();
    button(QDialogButtonBox::Apply)->setEnabled(false);
    dataChanged = false;
}

void COptionDialog::slotChanged()
{
    button(QDialogButtonBox::Apply)->setEnabled(true);
    dataChanged = true;
}

void COptionDialog::slotHelp()
{
    KHelpClient::invokeHelp(QLatin1String(""), QStringLiteral("kdf"));
}

#include "moc_optiondialog.cpp"
