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

#include "paintingpanel.h"
#include "paintingmodule.h"
#include <QtGui/QCheckBox>
#include <QtGui/QStyleFactory>
#include <QtGui/QVBoxLayout>

using namespace Inspector::Internal;

PaintingPanel::PaintingPanel(PaintingModule *module)
  : AbstractPanel(module)
  , m_paintingModule(module)
{
    setupUi(this);

    connect(debugBox, SIGNAL(toggled(bool)),
            this, SLOT(slotDebugPaintingToggled(bool)));

    foreach (const QString &styleName, QStyleFactory::keys()) {
        QListWidgetItem *styleItem = new QListWidgetItem(styleListWidget);
        styleItem->setText(styleName);
    }

    connect(styleListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotStyleSelectionChanged()));
    slotStyleSelectionChanged();

    connect(styleBox, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(slotApplyStyle()));
}

void PaintingPanel::slotDebugPaintingToggled(bool enabled)
{
    m_paintingModule->setShowExposedAreas(enabled);
}

void PaintingPanel::slotStyleSelectionChanged()
{
    styleBox->setEnabled(!styleListWidget->selectedItems().isEmpty());
}

void PaintingPanel::slotApplyStyle()
{
    if (QListWidgetItem *item = styleListWidget->currentItem())
        m_paintingModule->setGuiStyle(item->text());
}
