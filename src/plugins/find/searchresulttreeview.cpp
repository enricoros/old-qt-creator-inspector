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

#include "searchresulttreeview.h"
#include "searchresulttreeitemroles.h"
#include "searchresulttreemodel.h"
#include "searchresulttreeitemdelegate.h"

#include <QtGui/QHeaderView>

using namespace Find::Internal;

SearchResultTreeView::SearchResultTreeView(QWidget *parent)
    : QTreeView(parent)
    , m_model(new SearchResultTreeModel(this))
    , m_autoExpandResults(false)
{
    setModel(m_model);
    setItemDelegate(new SearchResultTreeItemDelegate(this));
    setIndentation(14);
    setUniformRowHeights(true);
    header()->hide();

    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(emitJumpToSearchResult(QModelIndex)));
}

void SearchResultTreeView::setAutoExpandResults(bool expand)
{
    m_autoExpandResults = expand;
}

void SearchResultTreeView::setTextEditorFont(const QFont &font)
{
    m_model->setTextEditorFont(font);
}

void SearchResultTreeView::clear()
{
    m_model->clear();
}

void SearchResultTreeView::appendResultLine(int index, const QString &fileName, int lineNumber, const QString &rowText,
                                            int searchTermStart, int searchTermLength)
{
    int insertionIndex = m_model->addResultLine(index, fileName, lineNumber, rowText,
                                                searchTermStart, searchTermLength);

    if (m_autoExpandResults && insertionIndex != -1)
        setExpanded(model()->index(insertionIndex, 0), true);
}

void SearchResultTreeView::emitJumpToSearchResult(const QModelIndex &index)
{
    if (model()->data(index, ItemDataRoles::TypeRole).toString().compare("row") != 0)
        return;

    int position = model()->data(index, ItemDataRoles::ResultIndexRole).toInt();
    int checked = model()->data(index, Qt::CheckStateRole).toBool();

    emit jumpToSearchResult(position, checked);
}

void SearchResultTreeView::keyPressEvent(QKeyEvent *e)
{
    if (!e->modifiers() && e->key() == Qt::Key_Return) {
        emit activated(currentIndex());
        e->accept();
        return;
    }
    QTreeView::keyPressEvent(e);
}

SearchResultTreeModel *SearchResultTreeView::model() const
{
    return m_model;
}
