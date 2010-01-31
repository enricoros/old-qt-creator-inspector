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

#ifndef TARGETWINDOW_H
#define TARGETWINDOW_H

#include <QtGui/QWidget>

namespace Inspector {

class Instance;

namespace Internal {

class ComboTreeWidget;
class PanelContainerWidget;
class StatusBarWidget;

class TargetWindow : public QWidget
{
    Q_OBJECT

public:
    TargetWindow(Inspector::Instance *instance, QWidget *parent = 0);

    Inspector::Instance *targetInstance() const;

signals:
    void requestTargetDisplay();

private slots:
    void slotMenuChanged(const QStringList &path, const QVariant &data);
    void slotSetCurrentPanel(int moduleUid, int panelId);

private:
    void setInstance(Inspector::Instance *instance);
    void showPanel(int moduleUid, int panelId);
    Instance *              m_instance;
    ComboTreeWidget *       m_menuWidget;
    PanelContainerWidget *  m_panelContainer;
    StatusBarWidget *       m_statusbarWidget;
};

} // namespace Internal
} // namespace Inspector

#endif // TARGETWINDOW_H
