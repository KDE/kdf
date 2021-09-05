/*
    kdfsortproxymodel.cpp

    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
