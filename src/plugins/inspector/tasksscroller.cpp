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

#include "tasksscroller.h"
#include "tasksmodel.h"
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsWidget>
#include <QPalette>
#include <QScrollBar>

using namespace Inspector::Internal;

// the scene drawing the tasks

TasksScroller::TasksScroller(QWidget *parent)
  : QGraphicsView(parent)
  , m_tasksModel(0)
  , m_scene(new TasksScene)
{
    // customize widget
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    setFrameStyle(QFrame::NoFrame);
    setScene(m_scene);
}

void TasksScroller::setTasksModel(TasksModel *model)
{
    // clear previous model data
    if (m_tasksModel) {
        // forget previous model
        disconnect(m_tasksModel, 0, this, 0);
    }

    // set new model
    m_tasksModel = model;

    // handle new model's data
    if (m_tasksModel) {
        // use current model
        connect(m_tasksModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotTasksChanged()));
        connect(m_tasksModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotTasksChanged()));
        connect(m_tasksModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotTasksChanged()));
    }
}

QSize TasksScroller::sizeHint() const
{
    return minimumSizeHint();
}

QSize TasksScroller::minimumSizeHint() const
{
    return QSize(200, TasksScene::fixedHeight());
}

void TasksScroller::slotStopTask(quint32 tid)
{
    if (m_tasksModel)
        m_tasksModel->requestStopTask(tid);
}

void TasksScroller::slotTasksChanged()
{
    QList<quint32> newTasks = m_tasksModel->activeTasksId();

    // notify exceeding tasks
    QList<quint32>::iterator it = m_activeTasks.begin();
    while (it != m_activeTasks.end()) {
        quint32 tid = *it;
        if (!newTasks.removeAll(tid)) {
            it = m_activeTasks.erase(it);
            emit removeActiveTask(tid);
            continue;
        }
        ++it;
    }

    // notify new tasks
    foreach (quint32 tid, newTasks) {
        m_activeTasks.append(tid);
        emit newActiveTask(tid, m_tasksModel->taskName(tid));
    }
}

TasksScene::TasksScene(QObject * parent)
  : QGraphicsScene(parent)
  , m_scrollLocked(true)
  , m_pixelPerSecond(10)
{
    m_updateTimer.start(100, this);
}

int TasksScene::fixedHeight()
{
    return 20;
}

void TasksScene::clear()
{
    // remove all items
    foreach (QGraphicsItem * item, items()) {
        removeItem(item);
        delete item;
    }

    // clear start time
    m_startTime.start();
}

bool TasksScene::scrollLocked() const
{
    return m_scrollLocked;
}

void TasksScene::setScrollLocked(bool locked)
{
    if (locked != m_scrollLocked) {
        m_scrollLocked = locked;
        if (m_scrollLocked)
            regenScene();
    }
}

int TasksScene::pixelPerSecond() const
{
    return m_pixelPerSecond;
}

void TasksScene::setPixelPerSecond(int pps)
{
    if (pps != m_pixelPerSecond) {
        m_pixelPerSecond = pps;
        regenScene();
    }
}

void TasksScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    int sceneWidth = sceneRect().width();
    if (sceneWidth < 1)
        return;
    qreal scrollPercent = event->scenePos().x() / (qreal)sceneWidth;
    if (scrollPercent < 0.95) {
        m_scrollLocked = false;
        TasksScroller * widget = dynamic_cast<TasksScroller *>(views().first());
        int newVal = (int)(scrollPercent * (qreal)widget->horizontalScrollBar()->maximum());
        widget->horizontalScrollBar()->setValue(newVal);
    } else
        m_scrollLocked = true;
    event->accept();
}

void TasksScene::timerEvent(QTimerEvent * event)
{
    if (event->timerId() != m_updateTimer.timerId())
        return QGraphicsScene::timerEvent(event);

    if (m_startTime.isNull()) {
        m_startTime.start();
        return;
    }

    updateCurrentScene();
}

void TasksScene::regenScene()
{

}

class Inspector::Internal::TaskRectangle : public QGraphicsWidget
{
    public:
        TaskRectangle(int start, QGraphicsItem * parent = 0)
          : QGraphicsWidget(parent)
        {
            setPos(start - 1, 2);
            resize(2, TasksScene::fixedHeight() - 4);
            m_brush = QColor::fromHsv(60 * (qrand() % 6), 255, 200, 128);
        }

        void setEnd(int end)
        {
            resize(end - (int)x() + 1, TasksScene::fixedHeight() - 4);
            int pInc = qrand() % 4;
            if (m_percs.isEmpty())
                m_percs.append(pInc);
            else
                m_percs.append(qMin(100, m_percs.last() + pInc));
            update();
        }

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
        {
            Q_UNUSED(option);
            Q_UNUSED(widget);
            QRect rect = boundingRect().toRect().adjusted(0, 0, -1, -1);
            if (rect.width() > 2) {
                painter->setRenderHint(QPainter::Antialiasing, false);
                painter->setPen(QPen(Qt::lightGray, 1));
                painter->setBrush(m_brush);
                painter->drawRect(rect);

                // draw the percent polygon
                int h = size().height();
                int x = 0;
                foreach (int val, m_percs) {
                    int y = h * (0.9 - 0.8*(qreal)val / 100.0);
                    painter->fillRect(++x, y, 1, h - y - 1, QColor(255, 255, 255, 64));
                }

                QRect r = rect.adjusted(2, 1, -2, 1);
                if (r.width() > 2) {
                    QFont font = painter->font();
                    font.setPointSize(font.pointSize() - 2);
                    painter->setFont(font);
                    painter->setPen(Qt::black);
                    painter->setBrush(Qt::NoBrush);
                    painter->setRenderHint(QPainter::TextAntialiasing, true);
                    painter->drawText(r.adjusted(1, 1, 1, 1), Qt::AlignVCenter, tr("..."));
                    painter->setPen(Qt::white);
                    painter->drawText(r, Qt::AlignVCenter, tr("..."));
                }
            }
        }

    private:
        QBrush m_brush;
        QList<int> m_percs;
};

void TasksScene::updateCurrentScene()
{
    if (!m_startTime.isValid())
        return;
    QList<QGraphicsView *> viewports = views();
    int minWidth = viewports.isEmpty() ? 0 : viewports.first()->width();
    int contentsWidth = (m_startTime.elapsed() * m_pixelPerSecond) / 1000;
    setSceneRect(0, 0, qMax(contentsWidth, minWidth), fixedHeight());
    update();

    // if locked scrolling, update view's scrollbar too
    if (m_scrollLocked && !views().isEmpty()) {
        TasksScroller * widget = dynamic_cast<TasksScroller *>(views().first());
        widget->horizontalScrollBar()->setValue(contentsWidth);
    }

    // ### HACK AHEAD

    // RANDOM TASK DELETION
    if ((qrand() % 50) == 42 && !m_currentTasks.isEmpty()) {
        TaskRectangle * i = m_currentTasks.takeAt(qrand() % m_currentTasks.size());
        (void)i;
        //removeItem(i);
        //delete i;
    }

    // TASK UPDATE
    foreach (TaskRectangle * item, m_currentTasks)
        item->setEnd(contentsWidth);

    // RANDOM TASK ACTIVATION
    static bool first = true;
    if (first || (qrand() % 70) == 42) {
        first = false;
        TaskRectangle * i = new TaskRectangle(contentsWidth);
        addItem(i);
        m_currentTasks.append(i);
    }
}