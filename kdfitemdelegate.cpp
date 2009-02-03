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

#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QPainter>

#include <klocale.h>
#include <kcapacitybar.h>

#include "kdfwidget.h"
#include "kdfitemdelegate.h"

void KDFItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if ( index.column() == KDFWidget::UsageBarCol )
    {

        int progress = index.data( Qt::UserRole ).toInt();

        KCapacityBar bar;
        bar.setBarHeight( option.rect.height() - 2 );

        if( progress!=-1 )
        {
            bar.setValue( progress );
            bar.setText( QString::number( progress ) + "%" );
        }
        else
        {
            bar.setValue( 0 );
            bar.setEnabled( false );
            bar.setText( i18n("N/A") );
        }

        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        bar.drawCapacityBar( painter, option.rect );

    }
    else
    {

        QStyledItemDelegate::paint( painter, option, index );
    }
}

