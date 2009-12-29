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

#include "instanceview.h"

using namespace Inspector::Internal;

InstanceView::InstanceView(QWidget *parent)
  : QWidget(parent)
  , m_okPixmap(":/inspector/images/status-ok.png")
  , m_errorPixmap(":/inspector/images/status-err.png")
{
    setupUi(this);
    debugToolBox->hide();
}

void InstanceView::setFieldState(QWidget *field, int state)
{
    if (QCheckBox *c = dynamic_cast<QCheckBox *>(field)) {
        c->setChecked(state == 1);
        return;
    }

    if (QAbstractButton *b = dynamic_cast<QAbstractButton *>(field)) {
        switch (state) {
            case 0:
                b->setText(tr("?"));
                break;
            case 1:
                b->setIcon(m_okPixmap);
                if (b->isCheckable())
                    b->setChecked(true);
                break;
            case -1:
                b->setIcon(m_errorPixmap);
                if (b->isCheckable())
                    b->setChecked(false);
                break;
        }
    }

    if (QLabel *l = dynamic_cast<QLabel *>(field)) {
        switch (state) {
            case 0:
                l->setText(tr("unknown"));
                break;
            case 1:
                l->setPixmap(m_okPixmap);
                break;
            case -1:
                l->setPixmap(m_errorPixmap);
                break;
        }
    }
}
