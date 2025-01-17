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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtGui/QDialog>

#include "coreplugin/dialogs/ioptionspage.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
class QStandardItemModel;
class QStandardItem;
class QSortFilterProxyModel;
class QStackedLayout;
class QAbstractButton;
class QLineEdit;
class QLabel;
class QTreeView;
QT_END_NAMESPACE

namespace Utils {
    class FilterLineEdit;
}

namespace Core {
namespace Internal {

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent,
                   const QString &initialCategory = QString(),
                   const QString &initialPage = QString());
    ~SettingsDialog();

    // Run the dialog and return true if 'Ok' was chosen or 'Apply' was invoked
    // at least once
    bool execDialog();

    virtual QSize sizeHint() const;

public slots:
    void done(int);

private slots:
    void accept();
    void reject();
    void apply();
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void filter(const QString &text);

private:
    void createGui();
    void showPage(const QStandardItem *item);

    const QList<Core::IOptionsPage*> m_pages;

    QSet<Core::IOptionsPage*> m_visitedPages;
    QSortFilterProxyModel *m_proxyModel;
    QStandardItemModel *m_model;
    bool m_applied;
    QString m_currentCategory;
    QString m_currentPage;
    QStackedLayout *m_stackedLayout;
    Utils::FilterLineEdit *m_filterLineEdit;
    QTreeView *m_pageTree;
    QLabel *m_headerLabel;
};

} // namespace Internal
} // namespace Core

#endif // SETTINGSDIALOG_H
