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

#include <klocale.h>

#include "kdfconfig.h"
#include "mntconfig.h"

COptionDialog::COptionDialog( QWidget *parent )
        :KPageDialog( parent )
{
    setCaption( i18n("Configure") );
    setButtons( Help|Apply|Ok|Cancel );
    setDefaultButton( Ok );
    setFaceType( KPageDialog::Tabbed );
    setHelp( QLatin1String( "kcontrol/kdf/index.html" ), QString() );

    mConf = new KDFConfigWidget( this );
    connect( mConf, SIGNAL( configChanged() ), this, SLOT( slotChanged() ) );
    addPage( mConf, i18n("General Settings") );

    mMnt = new MntConfigWidget( this );
    connect( mMnt, SIGNAL( configChanged() ), this, SLOT( slotChanged() ) );
    addPage( mMnt, i18n("Mount Commands") );

    enableButton( Apply, false );
    dataChanged = false;
    connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
    connect(this,SIGNAL(applyClicked()),this,SLOT(slotApply()));
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
    enableButton( Apply, false );
    dataChanged = false;
}

void COptionDialog::slotChanged()
{
    enableButton( Apply, true );
    dataChanged = true;
}

#include "optiondialog.moc"

