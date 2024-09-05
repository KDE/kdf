/*
    SPDX-FileCopyrightText: 2009 Dario Andres Rodriguez <andresbajotierra@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDFITEMDELEGATE_H
#define KDFITEMDELEGATE_H

#include <QStyledItemDelegate>

class QPainter;

class KDFItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit KDFItemDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }
    ~KDFItemDelegate() override
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif
