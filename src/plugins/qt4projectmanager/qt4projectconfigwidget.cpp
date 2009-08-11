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
** contact the sales department at http://www.qtsoftware.com/contact.
**
**************************************************************************/

#include "qt4projectconfigwidget.h"

#include "makestep.h"
#include "qmakestep.h"
#include "qt4project.h"
#include "qt4projectmanagerconstants.h"
#include "qt4projectmanager.h"
#include "ui_qt4projectconfigwidget.h"

#include <coreplugin/icore.h>
#include <coreplugin/mainwindow.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <extensionsystem/pluginmanager.h>

#include <QtGui/QFileDialog>

namespace {
bool debug = false;
}

using namespace Qt4ProjectManager;
using namespace Qt4ProjectManager::Internal;

Qt4ProjectConfigWidget::Qt4ProjectConfigWidget(Qt4Project *project)
    : BuildConfigWidget(),
      m_pro(project)
{
    m_ui = new Ui::Qt4ProjectConfigWidget();
    m_ui->setupUi(this);

    // fix the layout
    m_browseButton = m_ui->shadowBuildDirEdit->buttonAtIndex(0);
#ifdef Q_OS_WIN
    m_browseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif
    m_ui->gridLayout->addWidget(m_browseButton, 4, 2);
    int minimumHeight = qMax(m_ui->qtVersionComboBox->sizeHint().height(), m_ui->manageQtVersionPushButtons->sizeHint().height());
    Qt::Alignment labelAlignment = Qt::Alignment(style()->styleHint(QStyle::SH_FormLayoutLabelAlignment));
    for (int i = 0; i < m_ui->gridLayout->rowCount(); ++i) {
        m_ui->gridLayout->setRowMinimumHeight(i, minimumHeight);
        QLayoutItem *item = m_ui->gridLayout->itemAtPosition(i, 0);
        if (item)
            item->setAlignment(labelAlignment);
    }

    m_ui->shadowBuildDirEdit->setPromptDialogTitle(tr("Shadow Build Directory"));
    m_ui->shadowBuildDirEdit->setExpectedKind(Core::Utils::PathChooser::Directory);
    m_ui->invalidQtWarningLabel->setVisible(false);

    m_ui->detailsWidget->setVisible(false);
    m_ui->titleLabel->setText("");

    connect(m_ui->detailsButton, SIGNAL(clicked()),
            this, SLOT(toggleDetails()));

    connect(m_ui->nameLineEdit, SIGNAL(textEdited(QString)),
            this, SLOT(changeConfigName(QString)));

    connect(m_ui->shadowBuildCheckBox, SIGNAL(clicked(bool)),
            this, SLOT(shadowBuildCheckBoxClicked(bool)));

    connect(m_ui->shadowBuildDirEdit, SIGNAL(beforeBrowsing()),
            this, SLOT(onBeforeBeforeShadowBuildDirBrowsed()));

    connect(m_ui->shadowBuildDirEdit, SIGNAL(changed(QString)),
            this, SLOT(shadowBuildLineEditTextChanged()));

    connect(m_ui->qtVersionComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(qtVersionComboBoxCurrentIndexChanged(QString)));

    connect(m_ui->toolChainComboBox, SIGNAL(activated(int)),
            this, SLOT(selectToolChain(int)));

    connect(m_ui->importLabel, SIGNAL(linkActivated(QString)),
            this, SLOT(importLabelClicked()));

    connect(m_ui->manageQtVersionPushButtons, SIGNAL(clicked()),
            this, SLOT(manageQtVersions()));

    QtVersionManager *vm = QtVersionManager::instance();

    connect(vm, SIGNAL(qtVersionsChanged()),
            this, SLOT(setupQtVersionsComboBox()));
}

Qt4ProjectConfigWidget::~Qt4ProjectConfigWidget()
{
    delete m_ui;
}

void Qt4ProjectConfigWidget::toggleDetails()
{
    m_ui->detailsWidget->setVisible(!m_ui->detailsWidget->isVisible());
    fixupLayout(m_ui->detailsWidget);
}

void Qt4ProjectConfigWidget::updateDetails()
{
    QtVersion *version = m_pro->qtVersion(m_buildConfiguration);
    QString versionString;
    if (m_pro->qtVersionId(m_buildConfiguration) == 0) {
        versionString = tr("Default Qt Version");
    } else {
        versionString = version->name();
    }
    // Qt Version, Build Directory and Toolchain
    m_ui->titleLabel->setText(tr("using Qt Version: <b>%1</b><br>"
                                 "with Toolchain <b>%2</b><br>"
                                 "building in <b>%3</b>")
                              .arg(versionString,
                                   ProjectExplorer::ToolChain::toolChainName(m_pro->toolChainType(m_buildConfiguration)),
                                   m_pro->buildDirectory(m_buildConfiguration)));
}

void Qt4ProjectConfigWidget::manageQtVersions()
{
    Core::ICore *core = Core::ICore::instance();
    core->showOptionsDialog(Constants::QT_CATEGORY, Constants::QTVERSION_PAGE);
}


QString Qt4ProjectConfigWidget::displayName() const
{
    return tr("General");
}

void Qt4ProjectConfigWidget::init(const QString &buildConfiguration)
{
    if (debug)
        qDebug() << "Qt4ProjectConfigWidget::init()";

    m_buildConfiguration = buildConfiguration;

    m_ui->nameLineEdit->setText(m_pro->displayNameFor(m_buildConfiguration));

    setupQtVersionsComboBox();

    bool shadowBuild = m_pro->value(buildConfiguration, "useShadowBuild").toBool();
    m_ui->shadowBuildCheckBox->setChecked(shadowBuild);
    m_ui->shadowBuildDirEdit->setEnabled(shadowBuild);
    m_browseButton->setEnabled(shadowBuild);
    m_ui->shadowBuildDirEdit->setPath(m_pro->buildDirectory(buildConfiguration));
    updateImportLabel();
    updateToolChainCombo();
    updateDetails();
}

void Qt4ProjectConfigWidget::changeConfigName(const QString &newName)
{
    m_pro->setDisplayNameFor(m_buildConfiguration, newName);
}

void Qt4ProjectConfigWidget::setupQtVersionsComboBox()
{
    if (m_buildConfiguration.isEmpty()) // not yet initialized
        return;

    disconnect(m_ui->qtVersionComboBox, SIGNAL(currentIndexChanged(QString)),
        this, SLOT(qtVersionComboBoxCurrentIndexChanged(QString)));

    m_ui->qtVersionComboBox->clear();
    m_ui->qtVersionComboBox->addItem(tr("Default Qt Version"), 0);

    if (m_pro->qtVersionId(m_buildConfiguration) == 0) {
        m_ui->qtVersionComboBox->setCurrentIndex(0);
        m_ui->invalidQtWarningLabel->setVisible(false);
    }
    // Add Qt Versions to the combo box
    QtVersionManager *vm = QtVersionManager::instance();
    const QList<QtVersion *> &versions = vm->versions();
    for (int i = 0; i < versions.size(); ++i) {
        m_ui->qtVersionComboBox->addItem(versions.at(i)->name(), versions.at(i)->uniqueId());

        if (versions.at(i)->uniqueId() == m_pro->qtVersionId(m_buildConfiguration)) {
            m_ui->qtVersionComboBox->setCurrentIndex(i + 1);
            m_ui->invalidQtWarningLabel->setVisible(!versions.at(i)->isValid());
        }
    }

    // And connect again
    connect(m_ui->qtVersionComboBox, SIGNAL(currentIndexChanged(QString)),
        this, SLOT(qtVersionComboBoxCurrentIndexChanged(QString)));
}

void Qt4ProjectConfigWidget::onBeforeBeforeShadowBuildDirBrowsed()
{
    QString initialDirectory = QFileInfo(m_pro->file()->fileName()).absolutePath();
    if (!initialDirectory.isEmpty())
        m_ui->shadowBuildDirEdit->setInitialBrowsePathBackup(initialDirectory);
}

void Qt4ProjectConfigWidget::shadowBuildCheckBoxClicked(bool checked)
{
    m_ui->shadowBuildDirEdit->setEnabled(checked);
    m_browseButton->setEnabled(checked);
    bool b = m_ui->shadowBuildCheckBox->isChecked();
    m_pro->setValue(m_buildConfiguration, "useShadowBuild", b);
    if (b)
        m_pro->setValue(m_buildConfiguration, "buildDirectory", m_ui->shadowBuildDirEdit->path());
    else
        m_pro->setValue(m_buildConfiguration, "buildDirectory", QVariant(QString::null));
    updateDetails();
}

void Qt4ProjectConfigWidget::updateImportLabel()
{
    bool visible = false;

    // we only show if we actually have a qmake and makestep
    if (m_pro->qmakeStep() && m_pro->makeStep()) {
        QString qtPath = QtVersionManager::findQtVersionFromMakefile(m_pro->buildDirectory(m_buildConfiguration));
        QtVersion *version = m_pro->qtVersion(m_buildConfiguration);
        // check that there's a makefile
        if (!qtPath.isEmpty()) {
            // and that the makefile path is different from the current version
            if (qtPath != (version ? version->path() : QString())) {
                // import enable
                visible = true;
            } else {
                // check that the qmake flags, arguments match
                visible = !m_pro->compareBuildConfigurationToImportFrom(m_buildConfiguration, m_pro->buildDirectory(m_buildConfiguration));
            }
        } else {
            visible = false;
        }
    }

    m_ui->importLabel->setVisible(visible);
}

void Qt4ProjectConfigWidget::shadowBuildLineEditTextChanged()
{
    if (m_pro->value(m_buildConfiguration, "buildDirectory").toString() == m_ui->shadowBuildDirEdit->path())
        return;
    m_pro->setValue(m_buildConfiguration, "buildDirectory", m_ui->shadowBuildDirEdit->path());
    // if the directory already exists
    // check if we have a build in there and
    // offer to import it
    updateImportLabel();

    m_pro->invalidateCachedTargetInformation();
    updateDetails();
}

void Qt4ProjectConfigWidget::importLabelClicked()
{
    if (!m_pro->qmakeStep() || !m_pro->makeStep())
        return;
    QString directory = m_pro->buildDirectory(m_buildConfiguration);
    if (!directory.isEmpty()) {
        QString qtPath = QtVersionManager::findQtVersionFromMakefile(directory);
        if (!qtPath.isEmpty()) {
            QtVersionManager *vm = QtVersionManager::instance();
            QtVersion *version = vm->qtVersionForDirectory(qtPath);
            if (!version) {
                version = new QtVersion(QFileInfo(qtPath).baseName(), qtPath);
                vm->addVersion(version);
            }

            QPair<QtVersion::QmakeBuildConfig, QStringList> result =
                    QtVersionManager::scanMakeFile(directory, version->defaultBuildConfig());
            QtVersion::QmakeBuildConfig qmakeBuildConfig = result.first;
            QStringList additionalArguments = result.second;

            QString versionSpec = version->sourcePath() + "/mkspecs/" + version->mkspec();
            QString parsedSpec = Qt4Project::extractSpecFromArgumentList(additionalArguments);
            QString parsedSpecOrginal = parsedSpec;
            if (QFileInfo(parsedSpec).isRelative())
                parsedSpec = QDir::cleanPath(directory + "/" + parsedSpec);
            additionalArguments = Qt4Project::removeSpecFromArgumentList(additionalArguments);
            if (parsedSpec != versionSpec) {
                additionalArguments.prepend(parsedSpecOrginal);
                additionalArguments.prepend("-spec");
            }

            // So we got all the information now apply it...
            m_pro->setQtVersion(m_buildConfiguration, version->uniqueId());
            // Combo box will be updated at the end

            QMakeStep *qmakeStep = m_pro->qmakeStep();
            qmakeStep->setQMakeArguments(m_buildConfiguration, additionalArguments);
            MakeStep *makeStep = m_pro->makeStep();

            m_pro->setValue(m_buildConfiguration, "buildConfiguration", int(qmakeBuildConfig));
            // Adjust command line arguments, this is ugly as hell
            // If we are switching to BuildAll we want "release" in there and no "debug"
            // or "debug" in there and no "release"
            // If we are switching to not BuildAl we want neither "release" nor "debug" in there
            QStringList makeCmdArguments = makeStep->value(m_buildConfiguration, "makeargs").toStringList();
            bool debug = qmakeBuildConfig & QtVersion::DebugBuild;
            if (qmakeBuildConfig & QtVersion::BuildAll) {
                makeCmdArguments.removeAll(debug ? "release" : "debug");
                if (!makeCmdArguments.contains(debug ? "debug" : "release"))
                    makeCmdArguments.append(debug ? "debug" : "release");
            } else {
                makeCmdArguments.removeAll("debug");
                makeCmdArguments.removeAll("release");
            }
            makeStep->setMakeArguments(m_buildConfiguration, makeCmdArguments);
        }
    }
    setupQtVersionsComboBox();
    updateDetails();
}

void Qt4ProjectConfigWidget::qtVersionComboBoxCurrentIndexChanged(const QString &)
{
    //Qt Version
    int newQtVersion;
    if (m_ui->qtVersionComboBox->currentIndex() == 0) {
        newQtVersion = 0;
    } else {
        newQtVersion = m_ui->qtVersionComboBox->itemData(m_ui->qtVersionComboBox->currentIndex()).toInt();
    }
    QtVersionManager *vm = QtVersionManager::instance();
    bool isValid = vm->version(newQtVersion)->isValid();
    m_ui->invalidQtWarningLabel->setVisible(!isValid);
    if (newQtVersion != m_pro->qtVersionId(m_buildConfiguration)) {
        m_pro->setQtVersion(m_buildConfiguration, newQtVersion);
        updateToolChainCombo();
        m_pro->update();
    }
    updateDetails();
}

void Qt4ProjectConfigWidget::updateToolChainCombo()
{
    m_ui->toolChainComboBox->clear();
    QList<ProjectExplorer::ToolChain::ToolChainType> toolchains = m_pro->qtVersion(m_buildConfiguration)->possibleToolChainTypes();
    foreach (ProjectExplorer::ToolChain::ToolChainType toolchain, toolchains) {
        switch (toolchain) {
        case ProjectExplorer::ToolChain::GCC:
            m_ui->toolChainComboBox->addItem(tr("gcc"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::LinuxICC:
            m_ui->toolChainComboBox->addItem(tr("icc"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::MinGW:
            m_ui->toolChainComboBox->addItem(tr("mingw"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::MSVC:
            m_ui->toolChainComboBox->addItem(tr("msvc"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::WINCE:
            m_ui->toolChainComboBox->addItem(tr("wince"), qVariantFromValue(toolchain));
            break;
#ifdef QTCREATOR_WITH_S60
        case ProjectExplorer::ToolChain::WINSCW:
            m_ui->toolChainComboBox->addItem(tr("winscw"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::GCCE:
            m_ui->toolChainComboBox->addItem(tr("gcce"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::RVCT_ARMV5:
            m_ui->toolChainComboBox->addItem(tr("rvct - armv5"), qVariantFromValue(toolchain));
            break;
        case ProjectExplorer::ToolChain::RVCT_ARMV6:
            m_ui->toolChainComboBox->addItem(tr("rvct - armv6"), qVariantFromValue(toolchain));
            break;
#endif
        case ProjectExplorer::ToolChain::OTHER:
        case ProjectExplorer::ToolChain::INVALID:
        case ProjectExplorer::ToolChain::UNKNOWN:
            break;
        }
    }
    m_ui->toolChainComboBox->setEnabled(toolchains.size() > 1);
    setToolChain(toolchains.indexOf(m_pro->toolChainType(m_buildConfiguration)));
}

void Qt4ProjectConfigWidget::selectToolChain(int index)
{
    setToolChain(index);
    m_pro->update();
}

void Qt4ProjectConfigWidget::setToolChain(int index)
{
    ProjectExplorer::ToolChain::ToolChainType selectedToolChainType =
        m_ui->toolChainComboBox->itemData(index,
            Qt::UserRole).value<ProjectExplorer::ToolChain::ToolChainType>();
    m_pro->setToolChainType(m_buildConfiguration, selectedToolChainType);
    if (m_ui->toolChainComboBox->currentIndex() != index)
        m_ui->toolChainComboBox->setCurrentIndex(index);
    updateDetails();
}
