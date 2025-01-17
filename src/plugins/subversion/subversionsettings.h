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

#ifndef SUBVERSIONSETTINGS_H
#define SUBVERSIONSETTINGS_H

#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Subversion {
namespace Internal {

// Todo: Add user name and password?
struct SubversionSettings
{
    SubversionSettings();

    void fromSettings(QSettings *);
    void toSettings(QSettings *) const;

    inline int timeOutMS() const     { return timeOutS * 1000;  }
    inline int longTimeOutMS() const { return timeOutS * 10000; }

    // Add authentication and (maybe future) options to the
    // command line
    QStringList addOptions(const QStringList &args) const;
    // Format arguments for log windows hiding passwords, etc.
    static QString formatArguments(const QStringList &args);

    bool equals(const SubversionSettings &s) const;

    QString svnCommand;
    bool useAuthentication;
    QString user;
    QString password;
    int logCount;
    int timeOutS;
    bool promptToSubmit;
    bool spaceIgnorantAnnotation;
};

inline bool operator==(const SubversionSettings &p1, const SubversionSettings &p2)
    { return p1.equals(p2); }
inline bool operator!=(const SubversionSettings &p1, const SubversionSettings &p2)
    { return !p1.equals(p2); }

} // namespace Internal
} // namespace Subversion

#endif // SUBVERSIONSETTINGS_H
