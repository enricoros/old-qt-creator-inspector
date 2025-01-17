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

#ifndef IBACKENDMODULE_H
#define IBACKENDMODULE_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/QIcon>

namespace Inspector {
namespace Internal {

class AbstractPanel;
class IBackend;

/**
  \brief Describes the items to put in the menus by one module
*/
struct ModuleMenuEntry {
    QStringList path;
    QIcon icon;
    int moduleUid;
    int panelId;

    ModuleMenuEntry(const QStringList &path, int moduleUid, int panelId, const QIcon &icon = QIcon());
};

typedef QList<ModuleMenuEntry> ModuleMenuEntries;

/**
  \brief Encapsulates functionalities relative to a probing context

  Handles a certain type of tests, encapsulates communication, data (models) and panels
*/
class IBackendModule : public QObject
{
    Q_OBJECT

public:
    IBackendModule(IBackend *, QObject *parent = 0);

    // describe the module
    virtual int uid() const = 0;
    virtual QString name() const = 0;
    virtual ModuleMenuEntries menuEntries() const;
    virtual AbstractPanel *createPanel(int panelId);

    // TODO: make this protected?
    IBackend *parentBackend() const;

signals:
    void requestPanelDisplay(int panelId);

private:
    IBackend *m_backend;
};

} // namespace Internal
} // namespace Inspector

#endif // IBACKENDMODULE_H
