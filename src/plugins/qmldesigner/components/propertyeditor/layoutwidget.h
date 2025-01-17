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


#ifndef LAYOUTWIDGET_H
#define LAYOUTWIDGET_H

#include <QtGui/QFrame>
#include <QLabel>
#include <QPushButton>
#include <QUrl>

QT_BEGIN_NAMESPACE

class LayoutWidget : public QFrame
{
    Q_OBJECT

   Q_PROPERTY(bool leftAnchor READ leftAnchor WRITE setLeftAnchor NOTIFY leftAnchorChanged)
   Q_PROPERTY(bool rightAnchor READ rightAnchor WRITE setRightAnchor NOTIFY rightAnchorChanged)
   Q_PROPERTY(bool bottomAnchor READ bottomAnchor WRITE setBottomAnchor NOTIFY bottomAnchorChanged)
   Q_PROPERTY(bool topAnchor READ topAnchor WRITE setTopAnchor NOTIFY topAnchorChanged)

   Q_PROPERTY(QUrl leftButtonIcon READ icon WRITE setLeftButtonIcon)
   Q_PROPERTY(QUrl rightButtonIcon READ icon WRITE setRightButtonIcon)
   Q_PROPERTY(QUrl topButtonIcon READ icon WRITE setTopButtonIcon)
   Q_PROPERTY(QUrl bottomButtonIcon READ icon WRITE setBottomButtonIcon)

public:

    void setLeftButtonIcon(const QUrl &url)
    { setIcon(m_leftButton, url); }

    void setRightButtonIcon(const QUrl &url)
    { setIcon(m_rightButton, url); }

    void setTopButtonIcon(const QUrl &url)
    { setIcon(m_topButton, url); }

    void setBottomButtonIcon(const QUrl &url)
    { setIcon(m_bottomButton, url); }

    QUrl icon() const {return QUrl(); }

    LayoutWidget(QWidget *parent = 0);
    ~LayoutWidget();

    bool leftAnchor() const { return m_leftAnchor; }
    bool rightAnchor() const { return m_rightAnchor; }
    bool topAnchor() const { return m_topAnchor; }
    bool bottomAnchor() const { return m_bottomAnchor; }

public slots:
    void setLeftAnchor(bool anchor)
    {
        if (anchor == m_leftAnchor)
            return;
        m_leftAnchor = anchor;
        m_leftButton->setChecked(anchor);
        emit leftAnchorChanged();
    }

    void setRightAnchor(bool anchor)
    {
        if (anchor == m_rightAnchor)
            return;
        m_rightAnchor = anchor;
        m_rightButton->setChecked(anchor);
        emit rightAnchorChanged();
    }

    void setTopAnchor(bool anchor)
    {
        if (anchor == m_topAnchor)
            return;
        m_topAnchor = anchor;
        m_topButton->setChecked(anchor);
        emit topAnchorChanged();
    }

    void setBottomAnchor(bool anchor)
    {
        if (anchor == m_bottomAnchor)
            return;
        m_bottomAnchor = anchor;
        m_bottomButton->setChecked(anchor);
        emit bottomAnchorChanged();
    }

signals:
    //void colorChanged(const QColor &color);
    void fill();
    void topAnchorChanged();
    void bottomAnchorChanged();
    void leftAnchorChanged();
    void rightAnchorChanged();

private:
    void setIcon(QPushButton *button, QUrl url);
    bool m_leftAnchor, m_rightAnchor, m_topAnchor, m_bottomAnchor;
    QPushButton *m_leftButton;
    QPushButton *m_rightButton;
    QPushButton *m_topButton;
    QPushButton *m_bottomButton;
    QPushButton *m_middleButton;
};

QT_END_NAMESPACE

#endif

