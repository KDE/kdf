/*
 * mntconfig.h
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef __MNTCONFIG_H__
#define __MNTCONFIG_H__

#include <kcmodule.h>
#include <kconfig.h>
#include <kiconloader.h>

#include "disks.h"
#include "disklist.h"

class QGroupBox;
class QPushButton;
class QRadioButton;
class QLineEdit;
class KIconButton;

class CListView;
class QListViewItem;

/**************************************************************/

class MntConfigWidget : public QWidget
{
  Q_OBJECT

  public:
    enum ColType
    {
      ICONCOL=0,
      DEVCOL=1,
      MNTPNTCOL=2,
      MNTCMDCOL=3,
      UMNTCMDCOL=4
    };

  public:
    MntConfigWidget( QWidget *parent=0, const char *name=0, bool init=false );
    ~MntConfigWidget();

  public slots:
    void loadSettings( void );
    void applySettings( void );

  protected slots:
    void slotChanged();

  private slots:
    void readDFDone( void );
    void clicked( QListViewItem *item );
    void selectMntFile( void );
    void selectUmntFile( void );
    void iconChangedButton(QString);
    void iconChanged( const QString & );
    void mntCmdChanged( const QString & );
    void umntCmdChanged( const QString & );

  protected:       
    void closeEvent( QCloseEvent * );

  private:
    CListView   *mList;
    QGroupBox   *mGroupBox;
    QLineEdit   *mIconLineEdit;
    QLineEdit   *mMountLineEdit;
    QLineEdit   *mUmountLineEdit;
    QPushButton *mMountButton;
    QPushButton *mUmountButton;
    KIconButton *mIconButton;
    DiskList    mDiskList;
    bool        mInitializing;
    QMemArray<QListViewItem*> mDiskLookup;

  signals:
    void configChanged();
};


#endif
