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

#include "paintingmodule.h"
#include "instance.h"
#include "commserver.h"
#include "paintingmodel.h"
#include "painttemperatureview.h"

using namespace Inspector::Internal;

PaintingModule::PaintingModule(Inspector::Instance *instance, QObject *parent)
  : AbstractModule(instance, parent)
  , m_model(new PaintingModel)
{
    // read the data coming from the commserver
    connect(parentInstance()->commServer(), SIGNAL(incomingData(quint32,quint32,QByteArray*)),
            this, SLOT(slotProcessIncomingData(quint32,quint32,QByteArray*)));
}

PaintingModule::~PaintingModule()
{
    qDeleteAll(m_views);
    delete m_model;
}

PaintingModel *PaintingModule::model() const
{
    return m_model;
}

QString PaintingModule::name() const
{
    return tr("Painting Measures (0.6)");
}

ModuleMenuEntries PaintingModule::menuEntries() const
{
    ModuleMenuEntries entries;
    entries.append(ModuleMenuEntry(QStringList() << tr("Painting") << tr("Temperature"), Uid, 1, QIcon(":/inspector/images/menu-paintingtemperature.png")));
    entries.append(ModuleMenuEntry(QStringList() << tr("Painting") << tr("Pixel Energy"), Uid, 2, QIcon(":/inspector/images/menu-paintingenergy.png")));
    return entries;
}

AbstractView *PaintingModule::createView(int viewId)
{
    AbstractView *view = 0;
    if (viewId == 1) {
        view = new PaintTemperatureView(this);
    } else {
        qWarning("PaintingModule::createView: unknown view %d", viewId);
        return 0;
    }
    connect(view, SIGNAL(destroyed()), this, SLOT(slotViewDestroyed()));
    m_views.append(view);
    return view;
}

void PaintingModule::slotActivate()
{
    qWarning("PaintingModule::slotActivate: ACTIVATED");
}

void PaintingModule::slotDeactivate()
{
    qWarning("PaintingModule::slotDeactivate: DEACTIVATED");
    emit deactivated();
}

void PaintingModule::slotLock()
{
    foreach (AbstractView *view, m_views)
        view->setEnabled(false);
}

void PaintingModule::slotUnlock()
{
    foreach (AbstractView *view, m_views)
        view->setEnabled(true);
}

void PaintingModule::slotProcessIncomingData(quint32 channel, quint32 code1, QByteArray *data)
{
    // only filter comm by this Uid (NOTE: sync the probe impl)
    if (channel != 0x0002 /*Inspector::Internal::Channel_Painting*/)
        return;

    switch (code1) {
    case 1:     // begin
        break;
    case 2:     // end
        break;
    case 3:     // percent
        m_model->setPtProgress(qBound(0, QString(*data).toInt(), 100));
        break;
    case 4:     // start image
    case 5: {   // colored image
        QDataStream dataReader(data, QIODevice::ReadOnly);
        QSize size;
        quint32 format;
        QByteArray contents;
        dataReader >> size;
        dataReader >> format;
        dataReader >> contents;
        QImage image((uchar *)contents.data(), size.width(), size.height(), (QImage::Format)format);
        m_model->addResult(QDateTime::currentDateTime(), 1.0, "test", "test", QPixmap::fromImage(image));
        } break;
    default:
        qWarning("PaintingModule::slotProcessIncomingData: unhandled code1 %d", code1);
        break;
    }
}

void PaintingModule::slotViewDestroyed()
{
    AbstractView *view = static_cast<AbstractView *>(sender());
    m_views.removeAll(view);
}