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

#include <khelpclient.h>
#include <klocale.h>

COptionDialog::COptionDialog( QWidget *parent )
        :KPageDialog( parent )
{
    setWindowTitle( i18n("Configure") );
    setStandardButtons( QDialogButtonBox::Help|QDialogButtonBox::Apply|QDialogButtonBox::Ok|QDialogButtonBox::Cancel );
    setFaceType( KPageDialog::Tabbed );

    mConf = new KDFConfigWidget( this );
    connect( mConf, SIGNAL(configChanged()), this, SLOT(slotChanged()) );
    addPage( mConf, i18n("General Settings") );

    mMnt = new MntConfigWidget( this );
    connect( mMnt, SIGNAL(configChanged()), this, SLOT(slotChanged()) );
    addPage( mMnt, i18n("Mount Commands") );

    button(QDialogButtonBox::Apply)->setEnabled(false);
    dataChanged = false;
    connect(button(QDialogButtonBox::Ok), SIGNAL(clicked()),this,SLOT(slotOk()));
    connect(button(QDialogButtonBox::Apply), SIGNAL(clicked()),this,SLOT(slotApply()));
    connect(button(QDialogButtonBox::Help), SIGNAL(clicked()), this, SLOT(slotHelp()));
}


COptionDialog::~COptionDialog( void )
{
}


void COptionDialog::slotOk( void )
{
    if( dataChanged )
        slotApply();
    accept();
}


void COptionDialog::slotApply( void )
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

void COptionDialog::slotHelp( void )
{
    KHelpClient::invokeHelp(QLatin1String(""), QLatin1String("kdf"));
}

#include "optiondialog.moc"

