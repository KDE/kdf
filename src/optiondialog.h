/*
    SPDX-FileCopyrightText: 1999 Espen Sand espen@kde.org
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef COPTIONDIALOG_H
#define COPTIONDIALOG_H

#include "kdfprivate_export.h"

#include <KPageDialog>

class KDFConfigWidget;
class MntConfigWidget;

class KDFPRIVATE_EXPORT COptionDialog : public KPageDialog
{
    Q_OBJECT

public:
    explicit COptionDialog(QWidget *parent = nullptr);
    ~COptionDialog() override;

protected Q_SLOTS:
    void slotOk();
    void slotApply();
    void slotChanged();
    void slotHelp();

Q_SIGNALS:
    void valueChanged();

private:
    KDFConfigWidget *mConf = nullptr;
    MntConfigWidget *mMnt = nullptr;
    bool dataChanged;
};

#endif
