#ifndef QMLPROJECTITEM_H
#define QMLPROJECTITEM_H

#include <QObject>
#include <QStringList>
#include <qdeclarative.h>

namespace QmlProjectManager {

class QmlProjectContentItem : public QObject {
    // base class for all elements that should be direct children of Project element
    Q_OBJECT

public:
    QmlProjectContentItem(QObject *parent = 0) : QObject(parent) {}
};

class QmlProjectItemPrivate;

class QmlProjectItem : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlProjectItem)
    Q_DISABLE_COPY(QmlProjectItem)

    Q_PROPERTY(QDeclarativeListProperty<QmlProjectManager::QmlProjectContentItem> content READ content DESIGNABLE false)
    Q_PROPERTY(QString sourceDirectory READ sourceDirectory NOTIFY sourceDirectoryChanged)
    Q_PROPERTY(QStringList libraryPaths READ libraryPaths WRITE setLibraryPaths NOTIFY libraryPathsChanged)

    Q_CLASSINFO("DefaultProperty", "content");

public:
    QmlProjectItem(QObject *parent = 0);
    ~QmlProjectItem();

    QDeclarativeListProperty<QmlProjectContentItem> content();

    QString sourceDirectory() const;
    void setSourceDirectory(const QString &directoryPath);

    QStringList libraryPaths() const;
    void setLibraryPaths(const QStringList &paths);

    QStringList files() const;
    bool matchesFile(const QString &filePath) const;

signals:
    void qmlFilesChanged();
    void sourceDirectoryChanged();
    void libraryPathsChanged();

protected:
    QmlProjectItemPrivate *d_ptr;
};

} // namespace QmlProjectManager

QML_DECLARE_TYPE(QmlProjectManager::QmlProjectItem);
QML_DECLARE_TYPE(QmlProjectManager::QmlProjectContentItem);
Q_DECLARE_METATYPE(QList<QmlProjectManager::QmlProjectContentItem *>);

#endif // QMLPROJECTITEM_H
