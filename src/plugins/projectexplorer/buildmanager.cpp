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

#include "buildmanager.h"

#include "buildprogress.h"
#include "buildstep.h"
#include "compileoutputwindow.h"
#include "projectexplorerconstants.h"
#include "projectexplorer.h"
#include "project.h"
#include "projectexplorersettings.h"
#include "target.h"
#include "taskwindow.h"
#include "buildconfiguration.h"

#include <coreplugin/icore.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <coreplugin/progressmanager/futureprogress.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/qtcassert.h>

#include <QtCore/QDir>
#include <QtCore/QTimer>

#include <qtconcurrent/QtConcurrentTools>

#include <QtGui/QHeaderView>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

using namespace ProjectExplorer;
using namespace ProjectExplorer::Internal;

static inline QString msgProgress(int n, int total)
{
    return BuildManager::tr("Finished %n of %1 build steps", 0, n).arg(total);
}

BuildManager::BuildManager(ProjectExplorerPlugin *parent)
    : QObject(parent)
    , m_running(false)
    , m_previousBuildStepProject(0)
    , m_canceling(false)
    , m_maxProgress(0)
    , m_progressFutureInterface(0)
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    m_projectExplorerPlugin = parent;

    connect(&m_watcher, SIGNAL(finished()),
            this, SLOT(nextBuildQueue()));

    connect(&m_watcher, SIGNAL(progressValueChanged(int)),
            this, SLOT(progressChanged()));
    connect(&m_watcher, SIGNAL(progressRangeChanged(int, int)),
            this, SLOT(progressChanged()));

    m_outputWindow = new CompileOutputWindow(this);
    pm->addObject(m_outputWindow);

    m_taskWindow = new TaskWindow;
    pm->addObject(m_taskWindow);

    m_taskWindow->addCategory(Constants::TASK_CATEGORY_COMPILE, tr("Compile", "Category for compiler isses listened under 'Build Issues'"));
    m_taskWindow->addCategory(Constants::TASK_CATEGORY_BUILDSYSTEM, tr("Buildsystem", "Category for build system isses listened under 'Build Issues'"));

    connect(m_taskWindow, SIGNAL(tasksChanged()),
            this, SLOT(updateTaskCount()));

    connect(&m_progressWatcher, SIGNAL(canceled()),
            this, SLOT(cancel()));
    connect(&m_progressWatcher, SIGNAL(finished()),
            this, SLOT(finish()));
}

BuildManager::~BuildManager()
{
    cancel();
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();

    pm->removeObject(m_taskWindow);
    delete m_taskWindow;

    pm->removeObject(m_outputWindow);
    delete m_outputWindow;
}

bool BuildManager::isBuilding() const
{
    // we are building even if we are not running yet
    return !m_buildQueue.isEmpty() || m_running;
}

void BuildManager::cancel()
{
    if (m_running) {
        m_canceling = true;
        m_watcher.cancel();
        m_watcher.waitForFinished();

        // The cancel message is added to the output window via a single shot timer
        // since the canceling is likely to have generated new addToOutputWindow signals
        // which are waiting in the event queue to be processed
        // (And we want those to be before the cancel message.)
        QTimer::singleShot(0, this, SLOT(emitCancelMessage()));

        disconnect(m_currentBuildStep, SIGNAL(addTask(ProjectExplorer::TaskWindow::Task)),
                   this, SLOT(addToTaskWindow(ProjectExplorer::TaskWindow::Task)));
        disconnect(m_currentBuildStep, SIGNAL(addOutput(QString)),
                   this, SLOT(addToOutputWindow(QString)));
        decrementActiveBuildSteps(m_currentBuildStep->buildConfiguration()->target()->project());

        m_progressFutureInterface->setProgressValueAndText(m_progress*100, "Build canceled"); //TODO NBS fix in qtconcurrent
        clearBuildQueue();
    }
    return;
}

void BuildManager::updateTaskCount()
{
    Core::ProgressManager *progressManager = Core::ICore::instance()->progressManager();
    int errors = m_taskWindow->errorTaskCount();
    if (errors > 0) {
        progressManager->setApplicationLabel(QString("%1").arg(errors));
    } else {
        progressManager->setApplicationLabel("");
    }
    emit tasksChanged();
}

void BuildManager::finish()
{
    QApplication::alert(Core::ICore::instance()->mainWindow(), 3000);
}

void BuildManager::emitCancelMessage()
{
    emit addToOutputWindow(tr("<font color=\"#ff0000\">Canceled build.</font>"));
}

void BuildManager::clearBuildQueue()
{
    foreach (BuildStep *bs, m_buildQueue) {
        decrementActiveBuildSteps(bs->buildConfiguration()->target()->project());
        disconnect(bs, SIGNAL(addTask(ProjectExplorer::TaskWindow::Task)),
                   this, SLOT(addToTaskWindow(ProjectExplorer::TaskWindow::Task)));
        disconnect(bs, SIGNAL(addOutput(QString)),
                   this, SLOT(addToOutputWindow(QString)));
    }

    m_buildQueue.clear();
    m_running = false;
    m_previousBuildStepProject = 0;

    m_progressFutureInterface->reportCanceled();
    m_progressFutureInterface->reportFinished();
    m_progressWatcher.setFuture(QFuture<void>());
    delete m_progressFutureInterface;
    m_progressFutureInterface = 0;
    m_maxProgress = 0;

    emit buildQueueFinished(false);
}


void BuildManager::toggleOutputWindow()
{
    m_outputWindow->toggle(false);
}

void BuildManager::showTaskWindow()
{
    m_taskWindow->popup(false);
}

void BuildManager::toggleTaskWindow()
{
    m_taskWindow->toggle(false);
}

bool BuildManager::tasksAvailable() const
{
    return m_taskWindow->taskCount() > 0;
}

void BuildManager::gotoTaskWindow()
{
    m_taskWindow->popup(true);
}

void BuildManager::startBuildQueue()
{
    if (m_buildQueue.isEmpty()) {
        emit buildQueueFinished(true);
        return;
    }
    if (!m_running) {
        // Progress Reporting
        Core::ProgressManager *progressManager = Core::ICore::instance()->progressManager();
        m_progressFutureInterface = new QFutureInterface<void>;
        m_progressWatcher.setFuture(m_progressFutureInterface->future());
        progressManager->setApplicationLabel("");
        Core::FutureProgress *progress = progressManager->addTask(m_progressFutureInterface->future(),
              tr("Build"),
              Constants::TASK_BUILD,
              Core::ProgressManager::KeepOnFinish | Core::ProgressManager::ShowInApplicationIcon);
        connect(progress, SIGNAL(clicked()), this, SLOT(showBuildResults()));
        progress->setWidget(new BuildProgress(m_taskWindow));
        m_progress = 0;
        m_progressFutureInterface->setProgressRange(0, m_maxProgress * 100);

        m_running = true;
        m_canceling = false;
        m_progressFutureInterface->reportStarted();
        m_outputWindow->clearContents();
        m_taskWindow->clearTasks(Constants::TASK_CATEGORY_COMPILE);
        nextStep();
    } else {
        // Already running
        m_progressFutureInterface->setProgressRange(0, m_maxProgress * 100);
        m_progressFutureInterface->setProgressValueAndText(m_progress*100, msgProgress(m_progress, m_maxProgress));
    }
}

void BuildManager::showBuildResults()
{
    if (m_taskWindow->taskCount() != 0)
        toggleTaskWindow();
    else
        toggleOutputWindow();
    //toggleTaskWindow();
}

void BuildManager::addToTaskWindow(const ProjectExplorer::TaskWindow::Task &task)
{
    m_taskWindow->addTask(task);
}

void BuildManager::addToOutputWindow(const QString &string)
{
    m_outputWindow->appendText(string);
}

void BuildManager::nextBuildQueue()
{
    if (m_canceling)
        return;

    disconnect(m_currentBuildStep, SIGNAL(addTask(ProjectExplorer::TaskWindow::Task)),
               this, SLOT(addToTaskWindow(ProjectExplorer::TaskWindow::Task)));
    disconnect(m_currentBuildStep, SIGNAL(addOutput(QString)),
               this, SLOT(addToOutputWindow(QString)));

    ++m_progress;
    m_progressFutureInterface->setProgressValueAndText(m_progress*100, msgProgress(m_progress, m_maxProgress));
    decrementActiveBuildSteps(m_currentBuildStep->buildConfiguration()->target()->project());

    bool result = m_watcher.result();
    if (!result) {
        // Build Failure
        const QString projectName = m_currentBuildStep->buildConfiguration()->target()->displayName();
        addToOutputWindow(tr("<font color=\"#ff0000\">Error while building project %1</font>").arg(projectName));
        addToOutputWindow(tr("<font color=\"#ff0000\">When executing build step '%1'</font>").arg(m_currentBuildStep->displayName()));
        // NBS TODO fix in qtconcurrent
        m_progressFutureInterface->setProgressValueAndText(m_progress*100, tr("Error while building project %1").arg(projectName));
    }

    if (result)
        nextStep();
    else
        clearBuildQueue();
}

void BuildManager::progressChanged()
{
    if (!m_progressFutureInterface)
        return;
    int range = m_watcher.progressMaximum() - m_watcher.progressMinimum();
    if (range != 0) {
        int percent = (m_watcher.progressValue() - m_watcher.progressMinimum()) * 100 / range;
        m_progressFutureInterface->setProgressValue(m_progress * 100 + percent);
    }
}

void BuildManager::nextStep()
{
    if (!m_buildQueue.empty()) {
        m_currentBuildStep = m_buildQueue.front();
        m_buildQueue.pop_front();

        if (m_currentBuildStep->buildConfiguration()->target()->project() != m_previousBuildStepProject) {
            const QString projectName = m_currentBuildStep->buildConfiguration()->target()->displayName();
            addToOutputWindow(tr("<b>Running build steps for project %2...</b>")
                              .arg(projectName));
            m_previousBuildStepProject = m_currentBuildStep->buildConfiguration()->target()->project();
        }
        m_watcher.setFuture(QtConcurrent::run(&BuildStep::run, m_currentBuildStep));
    } else {
        m_running = false;
        m_previousBuildStepProject = 0;
        m_progressFutureInterface->reportFinished();
        m_progressWatcher.setFuture(QFuture<void>());
        delete m_progressFutureInterface;
        m_progressFutureInterface = 0;
        m_maxProgress = 0;
        emit buildQueueFinished(true);
    }
}

bool BuildManager::buildQueueAppend(BuildStep *bs)
{
    m_buildQueue.append(bs);
    ++m_maxProgress;
    incrementActiveBuildSteps(bs->buildConfiguration()->target()->project());

    connect(bs, SIGNAL(addTask(ProjectExplorer::TaskWindow::Task)),
            this, SLOT(addToTaskWindow(ProjectExplorer::TaskWindow::Task)));
    connect(bs, SIGNAL(addOutput(QString)),
            this, SLOT(addToOutputWindow(QString)));

    bool init = bs->init();
    if (!init) {
        const QString projectName = m_currentBuildStep->buildConfiguration()->target()->project()->displayName();
        const QString targetName = m_currentBuildStep->buildConfiguration()->target()->displayName();
        addToOutputWindow(tr("<font color=\"#ff0000\">Error while building project %1 (%2)</font>").arg(projectName, targetName));
        addToOutputWindow(tr("<font color=\"#ff0000\">When executing build step '%1'</font>").arg(m_currentBuildStep->displayName()));
        cancel();
        return false;
    }

    return true;
}

void BuildManager::buildProjects(const QList<BuildConfiguration *> &configurations)
{
    foreach(BuildConfiguration *bc, configurations) {
        QList<BuildStep *> buildSteps = bc->buildSteps();
        foreach (BuildStep *bs, buildSteps) {
            buildQueueAppend(bs);
        }
    }
    if (ProjectExplorerPlugin::instance()->projectExplorerSettings().showCompilerOutput)
        m_outputWindow->popup(false);
    startBuildQueue();
}

void BuildManager::cleanProjects(const QList<BuildConfiguration *> &configurations)
{
    foreach(BuildConfiguration *bc, configurations) {
        QList<BuildStep *> cleanSteps = bc->cleanSteps();
        foreach (BuildStep *bs, cleanSteps) {
            buildQueueAppend(bs);
        }
    }
    if (ProjectExplorerPlugin::instance()->projectExplorerSettings().showCompilerOutput)
        m_outputWindow->popup(false);
    startBuildQueue();
}

void BuildManager::buildProject(BuildConfiguration *configuration)
{
    buildProjects(QList<BuildConfiguration *>() << configuration);
}

void BuildManager::cleanProject(BuildConfiguration *configuration)
{
    cleanProjects(QList<BuildConfiguration *>() << configuration);
}

void BuildManager::appendStep(BuildStep *step)
{
    buildQueueAppend(step);
    startBuildQueue();
}

bool BuildManager::isBuilding(Project *pro)
{
    QHash<Project *, int>::iterator it = m_activeBuildSteps.find(pro);
    QHash<Project *, int>::iterator end = m_activeBuildSteps.end();
    if (it == end || *it == 0)
        return false;
    else
        return true;
}

void BuildManager::incrementActiveBuildSteps(Project *pro)
{
    QHash<Project *, int>::iterator it = m_activeBuildSteps.find(pro);
    QHash<Project *, int>::iterator end = m_activeBuildSteps.end();
    if (it == end) {
        m_activeBuildSteps.insert(pro, 1);
        emit buildStateChanged(pro);
    } else if (*it == 0) {
        ++*it;
        emit buildStateChanged(pro);
    } else {
        ++*it;
    }
}

void BuildManager::decrementActiveBuildSteps(Project *pro)
{
    QHash<Project *, int>::iterator it = m_activeBuildSteps.find(pro);
    QHash<Project *, int>::iterator end = m_activeBuildSteps.end();
    if (it == end) {
        Q_ASSERT(false && "BuildManager m_activeBuildSteps says project is not building, but apparently a build step was still in the queue.");
    } else if (*it == 1) {
        --*it;
        emit buildStateChanged(pro);
    } else {
        --*it;
    }
}
