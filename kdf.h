/*
  kdf.h - KDiskFree

  Copyright (C) 1998 by Michael Kropfberger <michael.kropfberger@gmx.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

  */

#ifndef __KDF_H__
#define __KDF_H__

#include <kmainwindow.h>
#include <kmenubar.h>

#include "kdfwidget.h"

/***************************************************************/
class KDFTopLevel : public KMainWindow
{
 Q_OBJECT

  public:
    KDFTopLevel(QWidget *parent=0, const char *name=0);
    ~KDFTopLevel() { };

  protected slots:
    virtual bool queryExit( void );
    void slotConfigureKeys();
  protected:
    KDFWidget* kdf;
};

#endif
