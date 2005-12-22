/*
 *   Copyright (C) 1999  Espen Sand, espen@kde.org
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

#ifndef _OPTION_DIALOG_H_
#define _OPTION_DIALOG_H_

#include <kdialogbase.h>

class KDFConfigWidget;
class MntConfigWidget;

class COptionDialog : public KDialogBase
{
  Q_OBJECT

  public:
    COptionDialog( QWidget *parent=0, const char *name=0, bool modal=true );
    ~COptionDialog( void );

  protected slots:
    virtual void slotOk( void );
    virtual void slotApply( void );
    void slotChanged();

  signals:
    void valueChanged( void );

  private:
    KDFConfigWidget *mConf;
    MntConfigWidget *mMnt;
    bool dataChanged;
};

#endif
