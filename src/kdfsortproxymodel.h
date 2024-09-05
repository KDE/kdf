/*
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDFSORTPROXYMODEL_H
#define KDFSORTPROXYMODEL_H

#include <QSortFilterProxyModel>

class KDFSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit KDFSortFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
    }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif
