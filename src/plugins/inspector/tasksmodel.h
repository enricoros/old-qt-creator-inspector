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

#ifndef TASKSMODEL_H
#define TASKSMODEL_H

#include "abstracteasymodel.h"
#include <QDateTime>

namespace Inspector {
namespace Internal {

class TaskItem : public QStandardItem
{
public:
    TaskItem(quint32 tid, const QString &name, const QString &description);

    // operations
    bool start();
    bool stop();
    void setProgress(int progress);

    // query status
    quint32 tid() const;
    QString name() const;
    QString description() const;
    bool isActive() const;
    bool isStarted() const;
    QDateTime startDate() const;
    bool isEnded() const;
    QDateTime endDate() const;
    qreal duration() const;
    int progress() const;

private:
    quint32 m_tid;
    QString m_name;
    QString m_description;
    bool m_started;
    QDateTime m_start;
    bool m_stopped;
    QDateTime m_stop;
    qreal m_duration;
    int m_progress;
};

class Q_DECL_EXPORT TasksModel : public AbstractEasyModel
{
    Q_OBJECT

public:
    TasksModel(QObject *parent = 0);

    // query tasks info
    int totalTasksCount() const;
    int activeTasksCount() const;
    QList<quint32> activeTasksId() const;
    QString taskName(quint32 taskId) const;

    // public operations for tasks
    void addTask(quint32 tid, const QString &name, const QString &description);
    void startTask(quint32 tid);
    void stopTask(quint32 tid);
    void killTask(quint32 tid) {stopTask(tid);}

private:
    TaskItem *task(quint32 tid) const;
    QStandardItem *tasksRoot() const;
};

} // namespace Internal
} // namespace Inspector

#endif // TASKSMODEL_H