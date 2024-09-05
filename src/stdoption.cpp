/*
    SPDX-FileCopyrightText: 1998 Michael Kropfberger <michael.kropfberger@gmx.net>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

/*
** Bug reports and questions can be sent to <kde-devel@kde.org>
*/

#include "stdoption.h"

#include <KConfigGroup>
#include <KSharedConfig>

QString CStdOption::mDefaultFileManager = QStringLiteral("dolphin %m");
int CStdOption::mDefaultUpdateFrequency = 60;

CStdOption::CStdOption()
{
    setDefault();
}

CStdOption::~CStdOption()
{
}

void CStdOption::updateConfiguration()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("KDFConfig"));
    mFileManager = config.readPathEntry("FileManagerCommand", mDefaultFileManager);
    mUpdateFrequency = config.readEntry("UpdateFrequency", mDefaultUpdateFrequency);
    mPopupIfFull = config.readEntry("PopupIfFull", true);
    mOpenFileManagerOnMount = config.readEntry("OpenFileMgrOnMount", false);
    mUseSystemFileManager = config.readEntry("UseSystemFileMgr", true);
}

void CStdOption::writeConfiguration()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("KDFConfig"));
    config.writeEntry("UpdateFrequency", mUpdateFrequency);
    config.writePathEntry("FileManagerCommand", mFileManager);
    config.writeEntry("PopupIfFull", mPopupIfFull);
    config.writeEntry("OpenFileMgrOnMount", mOpenFileManagerOnMount);
    config.writeEntry("UseSystemFileMgr", mUseSystemFileManager);
    config.sync();
}

void CStdOption::writeDefaultFileManager()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("KDFConfig"));
    config.writePathEntry("FileManagerCommand", mDefaultFileManager);
    config.sync();
}

QString CStdOption::fileManager() const
{
    return (mFileManager);
}

int CStdOption::updateFrequency() const
{
    return (mUpdateFrequency);
}

bool CStdOption::popupIfFull() const
{
    return (mPopupIfFull);
}

bool CStdOption::openFileManager() const
{
    return (mOpenFileManagerOnMount);
}

bool CStdOption::useSystemFileManager() const
{
    return (mUseSystemFileManager);
}

void CStdOption::setDefault()
{
    mFileManager = mDefaultFileManager;
    mUpdateFrequency = mDefaultUpdateFrequency;
    mPopupIfFull = true;
    mOpenFileManagerOnMount = false;
    mUseSystemFileManager = true;
}

void CStdOption::setFileManager(const QString &fileManager)
{
    mFileManager = fileManager;
}

void CStdOption::setUpdateFrequency(int frequency)
{
    mUpdateFrequency = frequency;
}

void CStdOption::setPopupIfFull(bool popupIfFull)
{
    mPopupIfFull = popupIfFull;
}

void CStdOption::setOpenFileManager(bool openFileManagerOnMount)
{
    mOpenFileManagerOnMount = openFileManagerOnMount;
}

void CStdOption::setUseSystemFileManager(bool useSystemFileManager)
{
    mUseSystemFileManager = useSystemFileManager;
}
