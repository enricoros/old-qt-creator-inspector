/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009-2010 Enrico Ros
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

#ifndef INSPECTORINSTANCE_H
#define INSPECTORINSTANCE_H

#include <QObject>
#include <QList>
#include <QVariantList>
#include "instancemodel.h"

namespace Inspector {


namespace Internal {

class CommServer;
class InspectorPlugin;
class ModuleController;
class NotificationWidget;

}

class Q_DECL_EXPORT Instance : public QObject
{
    Q_OBJECT

public:
    Instance(QObject *parent = 0);
    ~Instance();

    InstanceModel *model() const;

    Internal::CommServer *commServer() const;
    Internal::ModuleController *moduleController() const;

signals:
    void requestDisplay();

private slots:
    void slotNotificationTriggered();
    void slotNewWarnings(int count);

private:
    InstanceModel *m_model;
    Internal::CommServer *m_commServer;
    Internal::NotificationWidget *m_notification;
    Internal::ModuleController *m_moduleController;
    bool m_enabled;
    bool m_debugPaintFlag;
    bool m_sDebugging;
};

} // namespace Inspector

#endif // INSPECTORINSTANCE_H