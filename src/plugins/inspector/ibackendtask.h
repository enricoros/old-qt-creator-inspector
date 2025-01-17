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

#ifndef IBACKENDTASK_H
#define IBACKENDTASK_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Inspector {
namespace Internal {

class IBackend;

/**
  \brief An asynchronous/denyable/interruptible/finite time operation within a Backend

  How to implement a task:
   * upon start emit 'requestActivation' and be ready to receive an
     activation signal
   * when it's done, emit 'finished' and the task will be deleted at
     some point
*/
class IBackendTask : public QObject
{
    Q_OBJECT

public:
    IBackendTask(IBackend *, QObject *parent = 0);
    virtual ~IBackendTask();

    virtual QString displayName() const = 0;
    quint32 taskUid() const;

protected slots:
    virtual void activateTask() { }
    virtual void deactivateTask() { emit finished(); }
    virtual void lockTask() { }
    virtual void unlockTask() { }

signals:
    void requestActivation();
    void setProgress(int percent);
    void finished();

private:
    friend class IBackend;
    void controlActivate();
    void controlDeactivate();
    void controlRefuse();
    void controlWait();
    class IBackendTaskPrivate *d;
};

} // namespace Internal
} // namespace Inspector

#endif // IBACKENDTASK_H
