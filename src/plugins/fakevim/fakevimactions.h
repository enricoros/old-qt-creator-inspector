/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/

#ifndef FAKEVIM_ACTIONS_H
#define FAKEVIM_ACTIONS_H

#include <utils/savedaction.h>

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>

namespace FakeVim {
namespace Internal {

enum FakeVimSettingsCode
{
    ConfigUseFakeVim,
    ConfigStartOfLine,
    ConfigHlSearch,
    ConfigTabStop,
    ConfigSmartTab,
    ConfigShiftWidth,
    ConfigExpandTab,
    ConfigAutoIndent,
    ConfigSmartIndent,
    ConfigIncSearch,

    // indent  allow backspacing over autoindent
    // eol     allow backspacing over line breaks (join lines)
    // start   allow backspacing over the start of insert; CTRL-W and CTRL-U
    //         stop once at the start of insert.
    ConfigBackspace,

    // other actions
    SettingsDialog,
};

class FakeVimSettings : public QObject
{
    Q_OBJECT

public:
    FakeVimSettings();
    ~FakeVimSettings();
    void insertItem(int code, Utils::SavedAction *item,
        const QString &longname = QString(),
        const QString &shortname = QString());

    Utils::SavedAction *item(int code);
    Utils::SavedAction *item(const QString &name);

    void readSettings(QSettings *settings);
    void writeSettings(QSettings *settings);

private:
    QHash<int, Utils::SavedAction *> m_items;
    QHash<QString, int> m_nameToCode;
    QHash<int, QString> m_codeToName;
};

FakeVimSettings *theFakeVimSettings();
Utils::SavedAction *theFakeVimSetting(int code);

} // namespace Internal
} // namespace FakeVim

#endif // FAKEVIM_ACTTIONS_H
