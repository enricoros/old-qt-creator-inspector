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

#include "librarywizarddialog.h"
#include "filespage.h"
#include "libraryparameters.h"
#include "modulespage.h"

#include <utils/projectintropage.h>

#include <QtCore/QDebug>

#include <QtGui/QComboBox>
#include <QtGui/QLabel>

enum { debugLibWizard = 0 };
enum { IntroPageId, ModulesPageId, FilePageId };

namespace Qt4ProjectManager {
namespace Internal {

struct PluginBaseClasses {
    const char *name;
    const char *module;
    // blank separated list or 0
    const char *dependentModules;
    const char *targetDirectory;
};

static const PluginBaseClasses pluginBaseClasses[] =
{
    { "QAccessiblePlugin", "QtGui", "QtCore", "accessible" },
    { "QDecorationPlugin", "QtGui", "QtCore", 0},
    { "QFontEnginePlugin", "QtGui", "QtCore", 0},
    { "QIconEnginePluginV2", "QtGui", "QtCore", "imageformats" },
    { "QImageIOPlugin", "QtGui", "QtCore", "imageformats" },
    { "QScriptExtensionPlugin", "QtScript", "QtCore", 0 },
    { "QSqlDriverPlugin", "QtSql", "QtCore", "sqldrivers" },
    { "QStylePlugin", "QtGui", "QtCore", "styles" },
    { "QTextCodecPlugin", "QtCore", 0, "codecs" }
};

enum { defaultPluginBaseClass = 7 };

static const PluginBaseClasses *findPluginBaseClass(const QString &name)
{
    const int pluginBaseClassCount = sizeof(pluginBaseClasses)/sizeof(PluginBaseClasses);
    for (int i = 0; i < pluginBaseClassCount; i++)
        if (name == QLatin1String(pluginBaseClasses[i].name))
            return pluginBaseClasses + i;
    return 0;
}

// return dependencies of a plugin as a line ready for the 'QT=' line in a pro
// file
static QString pluginDependencies(const PluginBaseClasses *plb)
{
    QString dependencies;
    const QChar blank = QLatin1Char(' ');
    // Find the module names and convert to ids
    QStringList pluginModules= plb->dependentModules ?
                               QString(QLatin1String(plb->dependentModules)).split(blank) :
                               QStringList();
    pluginModules.push_back(QLatin1String(plb->module));
    foreach (const QString &module, pluginModules) {
        if (!dependencies.isEmpty())
            dependencies += blank;
        dependencies += ModulesPage::idOfModule(module);
    }
    return dependencies;
}

// A Project intro page with an additional type chooser.
class LibraryIntroPage : public Utils::ProjectIntroPage
{
    Q_DISABLE_COPY(LibraryIntroPage)
public:
    explicit LibraryIntroPage(QWidget *parent = 0);

    QtProjectParameters::Type type() const;

    virtual int nextId() const;

private:
    QComboBox *m_typeCombo;
};

LibraryIntroPage::LibraryIntroPage(QWidget *parent) :
    Utils::ProjectIntroPage(parent),
    m_typeCombo(new QComboBox)
{
    m_typeCombo->setEditable(false);
    m_typeCombo->addItem(LibraryWizardDialog::tr("Shared library"),
                         QVariant(QtProjectParameters::SharedLibrary));
    m_typeCombo->addItem(LibraryWizardDialog::tr("Statically linked library"),
                         QVariant(QtProjectParameters::StaticLibrary));
    m_typeCombo->addItem(LibraryWizardDialog::tr("Qt 4 plugin"),
                         QVariant(QtProjectParameters::Qt4Plugin));
    insertControl(0, new QLabel(LibraryWizardDialog::tr("Type")), m_typeCombo);
}


QtProjectParameters::Type LibraryIntroPage::type() const
{
    return static_cast<QtProjectParameters::Type>(m_typeCombo->itemData(m_typeCombo->currentIndex()).toInt());
}

int LibraryIntroPage::nextId() const
{
    // The modules page is skipped in the case of a plugin since it knows its
    // dependencies by itself
    const int rc = type() == QtProjectParameters::Qt4Plugin ? FilePageId : ModulesPageId;
    if (debugLibWizard)
        qDebug() << Q_FUNC_INFO <<  "returns" << rc;
    return rc;
}

// ------------------- LibraryWizardDialog
LibraryWizardDialog::LibraryWizardDialog(const QString &templateName,
                                         const QIcon &icon,
                                         const QList<QWizardPage*> &extensionPages,
                                         bool showModulesPage,
                                         QWidget *parent) :
    BaseQt4ProjectWizardDialog(showModulesPage, new LibraryIntroPage, IntroPageId, parent),
    m_filesPage(new FilesPage),
    m_pluginBaseClassesInitialized(false)
{
    setWindowIcon(icon);
    setWindowTitle(templateName);
    setSelectedModules(QLatin1String("core"));

    // Note that QWizard::currentIdChanged() is emitted at strange times.
    // Use the intro page instead, set up initially
    setIntroDescription(tr("This wizard generates a C++ library project."));

    addModulesPage(ModulesPageId);
    addTargetsPage();

    m_filesPage->setNamespacesEnabled(true);
    m_filesPage->setFormFileInputVisible(false);
    m_filesPage->setClassTypeComboVisible(false);
    setPage(FilePageId, m_filesPage);

    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)));

    foreach (QWizardPage *p, extensionPages)
        addPage(p);
}

void LibraryWizardDialog::setSuffixes(const QString &header, const QString &source,  const QString &form)
{
    m_filesPage->setSuffixes(header, source, form);
}

void LibraryWizardDialog::setLowerCaseFiles(bool l)
{
    m_filesPage->setLowerCaseFiles(l);
}

QtProjectParameters::Type  LibraryWizardDialog::type() const
{
    return static_cast<const LibraryIntroPage*>(introPage())->type();
}

QtProjectParameters LibraryWizardDialog::parameters() const
{
    QtProjectParameters rc;
    rc.type = type();
    rc.fileName = projectName();
    rc.path = path();
    if (rc.type == QtProjectParameters::Qt4Plugin) {
        // Plugin: Dependencies & Target directory
        if (const PluginBaseClasses *plb = findPluginBaseClass(m_filesPage->baseClassName())) {
            rc.selectedModules = pluginDependencies(plb);
            if (plb->targetDirectory) {
                rc.targetDirectory = QLatin1String("$$[QT_INSTALL_PLUGINS]/");
                rc.targetDirectory += QLatin1String(plb->targetDirectory);
            }
        }
    } else {
        // Modules from modules page
        rc.selectedModules = selectedModules();
        rc.deselectedModules = deselectedModules();
    }
    return rc;
}

void LibraryWizardDialog::slotCurrentIdChanged(int id)
{
    if (debugLibWizard)
        qDebug() << Q_FUNC_INFO << id;
    // Switching to files page: Set up base class accordingly (plugin)
    if (id != FilePageId)
        return;
    switch (type()) {
    case QtProjectParameters::Qt4Plugin:
        if (!m_pluginBaseClassesInitialized) {
             if (debugLibWizard)
                qDebug("initializing for plugins");
            QStringList baseClasses;
            const int pluginBaseClassCount = sizeof(pluginBaseClasses)/sizeof(PluginBaseClasses);
            Q_ASSERT(defaultPluginBaseClass < pluginBaseClassCount);
            for (int i = 0; i < pluginBaseClassCount; i++)
                baseClasses.push_back(QLatin1String(pluginBaseClasses[i].name));
            m_filesPage->setBaseClassChoices(baseClasses);
            m_filesPage->setBaseClassName(baseClasses.at(defaultPluginBaseClass));
            m_pluginBaseClassesInitialized = true;
        }
        m_filesPage->setBaseClassInputVisible(true);
        break;
    default: {
        // Urrm, figure out a good class name. Use project name this time
        QString className = projectName();
        if (!className.isEmpty())
            className[0] = className.at(0).toUpper();
        m_filesPage->setClassName(className);
        m_filesPage->setBaseClassInputVisible(false);
    }
        break;
    }
}

LibraryParameters LibraryWizardDialog::libraryParameters() const
{
    LibraryParameters rc;
    rc.className = m_filesPage->className();
    rc.baseClassName = m_filesPage->baseClassName();
    rc.sourceFileName = m_filesPage->sourceFileName();
    rc.headerFileName = m_filesPage->headerFileName();
    if (!rc.baseClassName.isEmpty())
        if (const PluginBaseClasses *plb = findPluginBaseClass(rc.baseClassName)) {
            rc.baseClassModule = QLatin1String(plb->module);
        }
    return rc;
}

} // namespace Internal
} // namespace Qt4ProjectManager
