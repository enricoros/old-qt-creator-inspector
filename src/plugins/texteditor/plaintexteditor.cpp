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

#include "plaintexteditor.h"
#include "tabsettings.h"
#include "texteditorconstants.h"
#include "texteditorplugin.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/uniqueidmanager.h>

using namespace TextEditor;
using namespace TextEditor::Internal;

PlainTextEditorEditable::PlainTextEditorEditable(PlainTextEditor *editor)
  : BaseTextEditorEditable(editor)
{
    Core::UniqueIDManager *uidm = Core::UniqueIDManager::instance();
    m_context << uidm->uniqueIdentifier(Core::Constants::K_DEFAULT_TEXT_EDITOR_ID);
    m_context << uidm->uniqueIdentifier(TextEditor::Constants::C_TEXTEDITOR);
}

PlainTextEditor::PlainTextEditor(QWidget *parent)
  : BaseTextEditor(parent)
{
    setRevisionsVisible(true);
    setMarksVisible(true);
    setRequestMarkEnabled(false);
    setLineSeparatorsAllowed(true);

    setMimeType(QLatin1String(TextEditor::Constants::C_TEXTEDITOR_MIMETYPE_TEXT));
    setDisplayName(tr(Core::Constants::K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME));
}

QList<int> PlainTextEditorEditable::context() const
{
    return m_context;
}

Core::IEditor *PlainTextEditorEditable::duplicate(QWidget *parent)
{
    PlainTextEditor *newEditor = new PlainTextEditor(parent);
    newEditor->duplicateFrom(editor());
    TextEditorPlugin::instance()->initializeEditor(newEditor);
    return newEditor->editableInterface();
}

QString PlainTextEditorEditable::id() const
{
    return QLatin1String(Core::Constants::K_DEFAULT_TEXT_EDITOR_ID);
}

// Indent a text block based on previous line.
// Simple text paragraph layout:
// aaaa aaaa
//
//   bbb bb
//   bbb bb
//
//  - list
//    list line2
//
//  - listn
//
// ccc
//
// @todo{Add formatting to wrap paragraphs. This requires some
// hoops as the current indentation routines are not prepared
// for additional block being inserted. It might be possible
// to do in 2 steps (indenting/wrapping)}
//

void PlainTextEditor::indentBlock(QTextDocument *doc, QTextBlock block, QChar typedChar)
{
    Q_UNUSED(typedChar)

    // At beginning: Leave as is.
    if (block == doc->begin())
        return;

    const QTextBlock previous = block.previous();
    const QString previousText = previous.text();
    // Empty line indicates a start of a new paragraph. Leave as is.
    if (previousText.isEmpty() || previousText.trimmed().isEmpty())
        return;

    // Just use previous line.
    // Skip blank characters when determining the indentation
    int i = 0;
    while (i < previousText.size()) {
        if (!previousText.at(i).isSpace()) {
            const TextEditor::TabSettings &ts = tabSettings();
            ts.indentLine(block, ts.columnAt(previousText, i));
            break;
        }
        ++i;
    }
}
