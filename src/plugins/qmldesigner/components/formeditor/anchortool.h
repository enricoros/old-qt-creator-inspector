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

#ifndef ANCHORTOOL_H
#define ANCHORTOOL_H

#include <QTimeLine>

#include "abstractformeditortool.h"

#include "anchorlineindicator.h"
#include "anchorindicator.h"
#include "anchormanipulator.h"

namespace QmlDesigner {

class AnchorLineHandleItem;

class AnchorTool : public QObject, public AbstractFormEditorTool
{
    Q_OBJECT
public:
    AnchorTool(FormEditorView* editorView);
    ~AnchorTool();

    void mousePressEvent(const QList<QGraphicsItem*> &itemList,
                         QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(const QList<QGraphicsItem*> &itemList,
                        QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(const QList<QGraphicsItem*> &itemList,
                           QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(const QList<QGraphicsItem*> &itemList,
                               QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(const QList<QGraphicsItem*> &itemList,
                        QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *keyEvent);

    void itemsAboutToRemoved(const QList<FormEditorItem*> &itemList);

    void selectedItemsChanged(const QList<FormEditorItem*> &itemList);

    void clear();

    void formEditorItemsChanged(const QList<FormEditorItem*> &itemList);

    static AnchorLineHandleItem* topAnchorLineHandleItem(const QList<QGraphicsItem*> & itemList);

private slots:
    void checkIfStillHovering();

private: //variables
    AnchorLineIndicator m_anchorLineIndicator;
    AnchorIndicator m_anchorIndicator;
    AnchorManipulator m_anchorManipulator;
    QTimeLine m_hoverTimeLine;
    QPointF m_lastMousePosition;
    AnchorLineHandleItem *m_lastAnchorLineHandleItem;
};

}
#endif // ANCHORTOOL_H
