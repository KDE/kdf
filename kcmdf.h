/*
  kcmdf.h - KDiskFree

  written 1999 by Michael Kropfberger <michael.kropfberger@gmx.net>
  
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
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   
  */


#ifndef __KCMDF_H__
#define __KCMDF_H__

#include "kdfwidget.h"
#include "kdfconfig.h"
#include "mntconfig.h"

#include <kcmodule.h>
#include <qtabwidget.h>

class KDiskFreeWidget : public KCModule
{
  Q_OBJECT
  
  public:
    KDiskFreeWidget( QWidget *parent=0, const char *name=0 );
    virtual void load( void );
    virtual void save( void );
    virtual void defaults( void );

  protected:
    virtual void hideEvent( QHideEvent * );

  private:
    int pageNumber( void );

  private:
    QTabWidget *mTab;
    QFrame *mPage[3];
    KDFWidget *mKdf;
    KDFConfigWidget *mMcw;
    MntConfigWidget *mKcw;
};

#endif
