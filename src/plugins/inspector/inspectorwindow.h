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

#ifndef INSPECTORWINDOW_H
#define INSPECTORWINDOW_H

#include <QtCore/QList>
#include <QtGui/QComboBox>
#include <QtGui/QScrollArea>
class QAbstractButton;
class QGridLayout;
class QLabel;

namespace ProjectExplorer {
class Project;
class RunConfiguration;
}

namespace Inspector {
namespace Internal {

class IFrameworkFactory;
class FrameworksComboBox;
class ProjectsComboBox;
class RunconfComboBox;

/**
  \brief The Main Window of Inspector. Can start Instances
*/
class InspectorWindow : public QScrollArea
{
    Q_OBJECT

public:
    InspectorWindow(QWidget *parent = 0);

    // start Inspector on a new Target
    void newTarget(ProjectExplorer::RunConfiguration *, IFrameworkFactory *);

signals:
    void requestDisplay();

private slots:
    void slotCreateTarget();
    void slotProjectChanged();

private:
    QAbstractButton *newInspectButton(int id);
    void appendWrappedWidget(const QString &title, const QIcon &icon, QWidget *widget);
    void appendSubWidget(QGridLayout *layout, QWidget *widget,
                         const QString &title = QString(),
                         const QString &subTitle = QString());
    QWidget *m_root;
    QGridLayout *m_layout;

    ProjectsComboBox *m_projectsCombo;
    QLabel *m_runconfLabel;
    RunconfComboBox *m_runconfCombo;
    FrameworksComboBox *m_frameworksCombo;
    QAbstractButton *m_newRunButton;
};

/**
  \brif A QComboBox synced with current projects
*/
class ProjectsComboBox : public QComboBox
{
    Q_OBJECT

public:
    ProjectsComboBox(QWidget *parent = 0);

    bool isEmpty() const;
    ProjectExplorer::Project *currentProject() const;

signals:
    void currentProjectChanged();

private slots:
    void add(ProjectExplorer::Project *);
    void remove(ProjectExplorer::Project *);
    void activeChanged(ProjectExplorer::Project *);

private:
    QList<ProjectExplorer::Project *> m_comboIndexToProject;
};

/**
  \brief A QComboBox synced with the RunConfigurations of a Project
*/
class RunconfComboBox : public QComboBox
{
    Q_OBJECT

public:
    RunconfComboBox(QWidget *parent = 0);

    void setProject(ProjectExplorer::Project *);
    ProjectExplorer::Project *currentProject() const;
    ProjectExplorer::RunConfiguration *currentRunConfiguration() const;

private slots:
    void add(ProjectExplorer::RunConfiguration *);
    void remove(ProjectExplorer::RunConfiguration *);
    void updateDisplayName();
    void activeChanged();

private:
    ProjectExplorer::Project *m_project;
};

/**
  \brief A QComboBox synced with the current IFrameworkFactories
*/
class FrameworksComboBox : public QComboBox
{
    Q_OBJECT

public:
    FrameworksComboBox(QWidget *parent = 0);

    IFrameworkFactory *currentFactory() const;
    static QList<IFrameworkFactory *> allFactories();

signals:
    void currentFrameworkChanged();
};

} // namespace Internal
} // namespace Inspector

#endif // INSPECTORWINDOW_H