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

#include <qmljs/parser/qmljsast_p.h>
#include <qmljs/parser/qmljsengine_p.h>

#include "changeimportsvisitor.h"

using namespace QmlJS;
using namespace QmlJS::AST;

using namespace QmlDesigner;
using namespace QmlDesigner::Internal;

ChangeImportsVisitor::ChangeImportsVisitor(TextModifier &textModifier,
                                           const QString &source):
        QMLRewriter(textModifier), m_source(source)
{}

bool ChangeImportsVisitor::add(QmlJS::AST::UiProgram *ast, const Import &import)
{
    setDidRewriting(false);
    if (!ast)
        return false;

    if (ast->imports && ast->imports->import) {
        int insertionPoint = 0;
        if (ast->members && ast->members->member) {
            insertionPoint = ast->members->member->firstSourceLocation().begin();
        } else {
            insertionPoint = m_source.length();
        }
        while (insertionPoint > 0) {
            --insertionPoint;
            const QChar c = m_source.at(insertionPoint);
            if (!c.isSpace() && c != QLatin1Char(';'))
                break;
        }
        replace(insertionPoint, 0, QLatin1String("\n") + import.toString(false));
    } else {
        replace(0, 0, import.toString(false) + QLatin1String("\n\n"));
    }

    setDidRewriting(true);

    return true;
}

bool ChangeImportsVisitor::remove(QmlJS::AST::UiProgram *ast, const Import &import)
{
    setDidRewriting(false);
    if (!ast)
        return false;

    for (UiImportList *iter = ast->imports; iter; iter = iter->next) {
        if (equals(iter->import, import)) {
            int start = iter->firstSourceLocation().begin();
            int end = iter->lastSourceLocation().end();
            includeSurroundingWhitespace(start, end);
            replace(start, end - start, QString());
            setDidRewriting(true);
        }
    }

    return didRewriting();
}

bool ChangeImportsVisitor::equals(QmlJS::AST::UiImport *ast, const Import &import)
{
    if (import.isLibraryImport()) {
        return flatten(ast->importUri) == import.url();
    } else if (import.isFileImport()) {
        return ast->fileName->asString() == import.file();
    } else {
        return false;
    }
}
