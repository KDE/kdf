/*
 * kdfconfig.h
 *
 * Copyright (c) 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


#ifndef __KDFCONFIG_H__
#define __KDFCONFIG_H__

#include <qmemarray.h>
#include <qstring.h>

#include <stdoption.h>

class QCheckBox;
class QLabel;
class QLCDNumber;
class QLineEdit;
class QListViewItem;
class QScrollBar;

class CListView;

class KDFConfigWidget : public QWidget
{
  Q_OBJECT

  class CTabName
  {
    public:
      CTabName( const QString &res, const QString &name )
      {
        mRes     = res;
        mName    = name;
      };
      CTabName( void ) { }
      ~CTabName( void ) { }

      QString mRes;
      QString mName;
  };

  public:
    KDFConfigWidget( QWidget *parent=0, const char *name=0, bool init=false);
    ~KDFConfigWidget();

  public slots:
    void loadSettings( void );
    void applySettings( void );
    void defaultsBtnClicked( void );

  protected slots:
    void slotChanged();

  private slots:
    void toggleListText( QListViewItem *item, const QPoint &, int column );

  protected:
    void closeEvent( QCloseEvent * );

  private:
    QMemArray<CTabName*> mTabName;
    CListView  *mList;
    QScrollBar *mScroll;
    QLCDNumber *mLCD;
    QLineEdit  *mFileManagerEdit;
    QCheckBox  *mOpenMountCheck;
    QCheckBox  *mPopupFullCheck;
    CStdOption mStd;

  signals:
    void configChanged();
};


#endif



