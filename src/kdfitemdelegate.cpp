/*
* kdfitemdelegate.cpp
*
* Copyright (c) 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>
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

