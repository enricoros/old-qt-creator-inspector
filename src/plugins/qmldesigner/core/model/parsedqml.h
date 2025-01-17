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

#ifndef PARSEDQML_H
#define PARSEDQML_H

#include <QtCore/QString>

#include <qmljsengine_p.h>
#include <qmljslexer_p.h>
#include <qmljsparser_p.h>
#include <qmljsnodepool_p.h>
#include <qmljsast_p.h>

namespace QmlDesigner {
namespace Internal {

class ParsedQML
{
public:
    explicit ParsedQML(const QString &sourceCode, const QString &fileName = QString());

    bool isValid() const { return m_ast != 0; }
    QmlJS::AST::UiProgram *ast() const { return m_ast; }

    QString sourceCode() const { return m_sourceCode; }

private:
    QString m_sourceCode;
    QmlJS::Engine m_engine;
    QmlJS::NodePool m_nodePool;
    QmlJS::AST::UiProgram* m_ast;
};

} // namespace Internal
} // namespace QmlDesigner

#endif // PARSEDQML_H
