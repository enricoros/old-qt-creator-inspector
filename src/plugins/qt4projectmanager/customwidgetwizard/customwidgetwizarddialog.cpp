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

#include "customwidgetwizarddialog.h"
#include "customwidgetwidgetswizardpage.h"
#include "customwidgetpluginwizardpage.h"
#include "customwidgetwizard.h"

namespace Qt4ProjectManager {
namespace Internal {

enum { IntroPageId, WidgetsPageId, PluginPageId };

CustomWidgetWizardDialog::CustomWidgetWizardDialog(const QString &templateName,
                                                   const QIcon &icon,
                                                   const QList<QWizardPage*> &extensionPages,
                                                   QWidget *parent) :
    ProjectExplorer::BaseProjectWizardDialog(parent),
    m_widgetsPage(new CustomWidgetWidgetsWizardPage),
    m_pluginPage(new CustomWidgetPluginWizardPage)

{
    setWindowIcon(icon);
    setWindowTitle(templateName);

    setIntroDescription(tr("This wizard generates a Qt4 Designer Custom Widget "
                           "or a Qt4 Designer Custom Widget Collection project."));

    setPage(WidgetsPageId, m_widgetsPage);
    setPage(PluginPageId, m_pluginPage);

    foreach (QWizardPage *p, extensionPages)
        addPage(p);
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)));
}

FileNamingParameters CustomWidgetWizardDialog::fileNamingParameters() const
{
    return m_widgetsPage->fileNamingParameters();
}

void CustomWidgetWizardDialog::setFileNamingParameters(const FileNamingParameters &fnp)
{
    m_widgetsPage->setFileNamingParameters(fnp);
    m_pluginPage->setFileNamingParameters(fnp);
}

void CustomWidgetWizardDialog::slotCurrentIdChanged(int id)
{
    switch (id) {
    case IntroPageId:
    case WidgetsPageId:
        break;
    case PluginPageId:
        m_pluginPage->init(m_widgetsPage);
        break;
    }
}

QSharedPointer<PluginOptions> CustomWidgetWizardDialog::pluginOptions() const
{
    QSharedPointer<PluginOptions> rc = m_pluginPage->basicPluginOptions();
    rc->widgetOptions = m_widgetsPage->widgetOptions();
    return rc;
}

} // namespace Internal
} // namespace Qt4ProjectManager
