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

#ifndef PERFORMANCEWINDOW_H
#define PERFORMANCEWINDOW_H

#include <QWidget>
#include "abstracttest.h"
class QComboBox;
class QVBoxLayout;

namespace Performance {
namespace Internal {
class TaskbarWidget;
class TestControl;
class ViewContainerWidget;

class PerformanceWindow : public QWidget
{
    Q_OBJECT

public:
    PerformanceWindow(TestControl * control, QWidget *parent = 0);

    /// displays the only hardcoded view. all the others are plugged in
    void showDefaultView();
    void showSubSelectorView();

private slots:
    void updateMainCombo();
    void slotMainComboChanged(int);
    void slotSubComboChanged(int);

private:
    void activateView(int testId, int viewId);

    TestControl *m_testControl;
    QComboBox *m_mainCombo;
    QComboBox *m_subCombo;
    ViewContainerWidget *m_viewWidget;
    TaskbarWidget *m_taskbarWidget;
    TestMenu m_mergedMenu;
};

} // namespace Internal
} // namespace Performance

#endif // PERFORMANCEWINDOW_H
