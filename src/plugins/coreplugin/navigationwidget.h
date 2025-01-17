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

#ifndef NAVIGATIONWIDGET_H
#define NAVIGATIONWIDGET_H

#include <coreplugin/minisplitter.h>

#include <QtGui/QComboBox>

QT_BEGIN_NAMESPACE
class QSettings;
class QShortcut;
class QToolButton;
QT_END_NAMESPACE

namespace Utils {
class StyledBar;
}

namespace Core {
class INavigationWidgetFactory;
class IMode;
class Command;

namespace Internal {
class NavigationWidget;
}

class CORE_EXPORT NavigationWidgetPlaceHolder : public QWidget
{
    friend class Core::Internal::NavigationWidget;
    Q_OBJECT
public:
    NavigationWidgetPlaceHolder(Core::IMode *mode, QWidget *parent = 0);
    ~NavigationWidgetPlaceHolder();
    static NavigationWidgetPlaceHolder* current();
    void applyStoredSize(int width);
private slots:
    void currentModeAboutToChange(Core::IMode *);
private:
    Core::IMode *m_mode;
    static NavigationWidgetPlaceHolder* m_current;
};

namespace Internal {

class NavigationSubWidget;

class NavigationWidget : public MiniSplitter
{
    Q_OBJECT
public:
    NavigationWidget(QAction *toggleSideBarAction);
    ~NavigationWidget();

    void saveSettings(QSettings *settings);
    void restoreSettings(QSettings *settings);

    bool isShown() const;
    void setShown(bool b);

    bool isSuppressed() const;
    void setSuppressed(bool b);

    static NavigationWidget* instance();

    int storedWidth();

    // Called from the place holders
    void placeHolderChanged(NavigationWidgetPlaceHolder *holder);

    QHash<QString, Core::Command*> commandMap() const { return m_commandMap; }

protected:
    void resizeEvent(QResizeEvent *);
private slots:
    void objectAdded(QObject*);
    void activateSubWidget();
    void splitSubWidget();
    void closeSubWidget();

private:
    void updateToggleText();
    NavigationSubWidget *insertSubItem(int position);
    QList<NavigationSubWidget *> m_subWidgets;
    QHash<QShortcut *, QString> m_shortcutMap;
    QHash<QString, Core::Command*> m_commandMap;
    bool m_shown;
    bool m_suppressed;
    int m_width;
    static NavigationWidget* m_instance;
    QAction *m_toggleSideBarAction;
};

class NavigationSubWidget : public QWidget
{
    Q_OBJECT
public:
    NavigationSubWidget(NavigationWidget *parentWidget);
    virtual ~NavigationSubWidget();

    INavigationWidgetFactory *factory();
    void setFactory(INavigationWidgetFactory *factory);
    void setFactory(const QString &id);
    void setFocusWidget();

    void saveSettings(int position);
    void restoreSettings(int position);

    Core::Command *command(const QString &title) const;

signals:
    void splitMe();
    void closeMe();

private slots:
    void objectAdded(QObject*);
    void aboutToRemoveObject(QObject*);
    void setCurrentIndex(int);

private:
    NavigationWidget *m_parentWidget;
    QComboBox *m_navigationComboBox;
    QWidget *m_navigationWidget;
    Utils::StyledBar *m_toolBar;
    QList<QToolButton *> m_additionalToolBarWidgets;
};

class NavComboBox : public QComboBox
{
    Q_OBJECT

public:
    NavComboBox(NavigationSubWidget *navSubWidget);

protected:
    bool event(QEvent *event);

private:
    NavigationSubWidget *m_navSubWidget;
};

} // namespace Internal
} // namespace Core

#endif // NAVIGATIONWIDGET_H
