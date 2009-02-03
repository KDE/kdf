/*
* kdfsortproxymodel.cpp
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

#include "kdfsortproxymodel.h"
#include "kdfwidget.h"

bool KDFSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{

    //Properly sort the file sizes using the numeric size instead of the string
    // representation (10Mb < 9Gb)
    if( left.column() == KDFWidget::SizeCol || left.column() == KDFWidget::FreeCol )
    {
        qulonglong leftData = sourceModel()->data( left, Qt::UserRole ).toULongLong();
        qulonglong rightData = sourceModel()->data( right, Qt::UserRole ).toULongLong();

        return leftData < rightData;
    }
    // Full percent can be -1 (non mounted disks)
    else if ( left.column() == KDFWidget::FullCol || left.column() == KDFWidget::UsageBarCol )
    {
        int leftData = sourceModel()->data( left, Qt::UserRole ).toInt();
        int rightData = sourceModel()->data( right, Qt::UserRole ).toInt();

        return leftData < rightData;

    }
    // Default sorting rules for string values
    else
    {
        return QSortFilterProxyModel::lessThan( left, right );
    }

}
