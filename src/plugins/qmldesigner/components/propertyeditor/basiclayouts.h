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

#ifndef BASICLAYOUTS_H
#define BASICLAYOUTS_H

#include <qlayoutobject.h>
#include <QHBoxLayout>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QBoxLayoutObject : public QLayoutObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QWidget> children READ children)

    Q_PROPERTY(int topMargin READ topMargin WRITE setTopMargin)
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin)
    Q_PROPERTY(int leftMargin READ leftMargin WRITE setLeftMargin)
    Q_PROPERTY(int rightMargin READ rightMargin WRITE setRightMargin)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)

    Q_CLASSINFO("DefaultProperty", "children")
public:
    QBoxLayoutObject(QObject *parent=0);
    explicit QBoxLayoutObject(QBoxLayout *, QObject *parent=0);
    virtual QLayout *layout() const;

    QDeclarativeListProperty<QWidget> children() {
        return QDeclarativeListProperty<QWidget>(this, 0, children_append, 0, 0, children_clear);
    }

private:
    friend class WidgetList;
    void addWidget(QWidget *);
    void clearWidget();

    static void children_append(QDeclarativeListProperty<QWidget> *property, QWidget *widget) {
        static_cast<QBoxLayoutObject*>(property->object)->addWidget(widget);
    }

    static void children_clear(QDeclarativeListProperty<QWidget> *property) {
        static_cast<QBoxLayoutObject*>(property->object)->clearWidget();
    }

    void getMargins()
    {
        _layout->getContentsMargins(&mLeft, &mTop, &mRight, &mBottom);
    }

    void setMargins()
    {
        _layout->setContentsMargins(mLeft, mTop, mRight, mBottom);
    }

    int topMargin()
    {
        getMargins();
        return mTop;
    }

    void setTopMargin(int margin)
    {
        getMargins();
        mTop = margin;
        setMargins();
    }

    int bottomMargin()
    {
        getMargins();
        return mBottom;
    }

    void setBottomMargin(int margin)
    {
        getMargins();
        mBottom = margin;
        setMargins();
    }

    int leftMargin()
    {
        getMargins();
        return mLeft;
    }

    void setLeftMargin(int margin)
    {
        getMargins();
        mLeft = margin;
        setMargins();
    }

    int rightMargin()
    {
        getMargins();
        return mRight;
    }

    void setRightMargin(int margin)
    {
        getMargins();
        mRight = margin;
        setMargins();
    }

    int spacing() const
    {
        return _layout->spacing();
    }

    void setSpacing(int spacing)
    {
        _layout->setSpacing(spacing);
    }

    QBoxLayout *_layout;

    int mTop, mLeft, mBottom, mRight;

};

class QHBoxLayoutObject : public QBoxLayoutObject
{
Q_OBJECT
public:
    QHBoxLayoutObject(QObject *parent=0);
};

class QVBoxLayoutObject : public QBoxLayoutObject
{
Q_OBJECT
public:
    QVBoxLayoutObject(QObject *parent=0);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QBoxLayoutObject);
QML_DECLARE_TYPE(QHBoxLayoutObject);
QML_DECLARE_TYPE(QVBoxLayoutObject);


class BasicLayouts {
public:
    static void registerDeclarativeTypes();
};

QT_END_HEADER

#endif // BASICLAYOUTS_H
