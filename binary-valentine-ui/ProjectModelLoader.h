#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QtQmlIntegration>

#include "ProjectModel.h"

namespace bv
{

class ProjectModelLoader : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    Q_INVOKABLE ProjectModel* createNewProjectModel();
    Q_INVOKABLE ProjectModel* createNewProjectModelFromProject(QString path);
    Q_INVOKABLE QString getLastError() const;

private:
    QString lastError_;
};

} // namespace bv
