/*
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kdfitemdelegate.h"

#include "kdfwidget.h"
#include "kdfutil.h"

#include <KCapacityBar>

#include <QBrush>
#include <QPainter>
#include <QPalette>
#include <QStyleOptionViewItem>

void KDFItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if ( index.column() == KDFWidget::UsageBarCol )
    {

        int progress = index.data( Qt::UserRole ).toInt();

        if( progress!=-1 )
        {
            KCapacityBar bar;
            bar.setBarHeight( option.rect.height() -2 );
            bar.setValue( progress );
            bar.setText(i18nc("Disk percentage", "%1%", progress));
            //Draw red bar on >=Full_Percent
            if ( progress >= Full_Percent )
            {
                QPalette p ( bar.palette() );
                p.setBrush( QPalette::Highlight, QBrush( Qt::red ) );
                bar.setPalette( p );
            }

            if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver)
                QStyledItemDelegate::paint( painter, option, index );

            QRect rect(option.rect);
            bar.drawCapacityBar( painter, rect.adjusted( 0, 0, -2, -1 ) );
        }
        else
        {
            QStyledItemDelegate::paint( painter, option, index );
        }

    }
    else
    {

        QStyledItemDelegate::paint( painter, option, index );
    }
}

