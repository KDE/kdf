/*
 *   Copyright (C) 1999  Espen Sand, espen@kde.org
 *                 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "optiondialog.h"

#include "kdfconfig.h"
#include "mntconfig.h"

#include <KHelpClient>

COptionDialog::COptionDialog( QWidget *parent )
        :KPageDialog( parent )
{
    setWindowTitle( i18n("Configure") );
    setStandardButtons( QDialogButtonBox::Help|QDialogButtonBox::Apply|QDialogButtonBox::Ok|QDialogButtonBox::Cancel );
    setFaceType( KPageDialog::Tabbed );

    mConf = new KDFConfigWidget( this );
    connect( mConf, &KDFConfigWidget::configChanged, this, &COptionDialog::slotChanged );
    addPage( mConf, i18n("General Settings") );

    mMnt = new MntConfigWidget( this );
    connect( mMnt, &MntConfigWidget::configChanged, this, &COptionDialog::slotChanged );
    addPage( mMnt, i18n("Mount Commands") );

    button(QDialogButtonBox::Apply)->setEnabled(false);
    dataChanged = false;
    connect(button(QDialogButtonBox::Ok), &QAbstractButton::clicked,this,&COptionDialog::slotOk);
    connect(button(QDialogButtonBox::Apply), &QAbstractButton::clicked,this,&COptionDialog::slotApply);
    connect(button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &COptionDialog::slotHelp);
}


COptionDialog::~COptionDialog()
{
}


void COptionDialog::slotOk()
{
    if( dataChanged )
        slotApply();
    accept();
}


void COptionDialog::slotApply()
{
    mConf->applySettings();
    mMnt->applySettings();
    emit valueChanged();
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


