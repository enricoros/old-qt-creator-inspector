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

#ifndef IMODE_H
#define IMODE_H

#include "icontext.h"

#include <coreplugin/core_global.h>

QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT IMode : public IContext
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
public:
    IMode(QObject *parent = 0);
    virtual ~IMode() {}

    virtual QString displayName() const = 0;
    virtual QIcon icon() const = 0;
    virtual int priority() const = 0;
    virtual QString id() const = 0;

    void setEnabled(bool enabled);
    bool isEnabled() const;

signals:
    void enabledStateChanged(bool enabled);

private:
    bool m_isEnabled;
};

} // namespace Core

#endif // IMODE_H
