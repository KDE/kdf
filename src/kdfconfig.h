/*
    kdfconfig.h

    SPDX-FileCopyrightText: 1999 Michael Kropfberger <michael.kropfberger@gmx.net>
    2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDFCONFIG_H
#define KDFCONFIG_H

#include "ui_kdfconfig.h"
#include "stdoption.h"
#include "kdfwidget.h"


class QCloseEvent;
class QPixmap;
class QTreeWidget;
class QTreeWidgetItem;

class Column;

class KDFConfigWidget : public QWidget,
                        private Ui_KDFConfigWidget
{
    Q_OBJECT

    public:
        explicit KDFConfigWidget( QWidget *parent=nullptr, bool init=false);
        ~KDFConfigWidget() override;

    public Q_SLOTS:
        void loadSettings( void );
        void applySettings( void );
        void defaultsBtnClicked( void );

    protected Q_SLOTS:
        void slotChanged();

    private Q_SLOTS:
        void toggleListText( QTreeWidgetItem *item, int column );

    protected:
        void closeEvent( QCloseEvent * ) override;

    private:
        CStdOption mStd;

        QPixmap iconVisible;
        QPixmap iconHidden;

        QList<Column> m_columnList;

    Q_SIGNALS:
        void configChanged();
};

#endif

