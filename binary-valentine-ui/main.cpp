#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickView>

#include "AnalysisProgress.h"
#include "BinaryValentineContext.h"
#include "RuleSelectorNode.h"
#include "ProjectSettingsNode.h"
#include "ProjectModel.h"
#include "ProjectModelLoader.h"
#include "ReportFilterNode.h"
#include "ReportOutputsNode.h"
#include "ScanRunnerNode.h"
#include "ScanTargetNode.h"
#include "TargetsNode.h"
#include "VersionInfo.h"

int main(int argc, char *argv[])
{
    auto versionInfo = std::make_unique<bv::VersionInfo>();

    //QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //qputenv( "QT_QUICK_BACKEND", "software" );
    //qputenv("QSG_RHI_BACKEND", "opengl");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);
    //QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);

    QGuiApplication app(argc, argv);

    QGuiApplication::setWindowIcon(QIcon(":/binary-valentine-ui/res/logo.png"));
    QGuiApplication::setApplicationName(versionInfo->getToolName());
    QGuiApplication::setApplicationVersion(versionInfo->getVersion());
    QGuiApplication::setOrganizationName(versionInfo->getAuthorName());

    QQmlApplicationEngine engine;

    qmlRegisterSingletonInstance("BinaryValentineNative", 1, 0,
                                 "VersionInfo", versionInfo.release());
    qmlRegisterSingletonInstance("BinaryValentineNative", 1, 0,
                                 "BinaryValentineContext", new bv::BinaryValentineContext());
    qmlRegisterType<bv::ProjectModel>("BinaryValentineNative", 1, 0,
                                      "ProjectModel");
    qmlRegisterType<bv::ProjectModelLoader>("BinaryValentineNative", 1, 0,
                                            "ProjectModelLoader");

    qmlRegisterUncreatableType<bv::RuleSelectorNode>(
        "BinaryValentineNative", 1, 0, "RuleSelector", "");
    qmlRegisterUncreatableType<bv::ProjectSettingsNode>(
        "BinaryValentineNative", 1, 0, "ProjectSettings", "");
    qmlRegisterUncreatableType<bv::ReportFilterNode>(
        "BinaryValentineNative", 1, 0, "ReportFilter", "");
    qmlRegisterUncreatableType<bv::TargetsNode>(
        "BinaryValentineNative", 1, 0, "Targets", "");
    qmlRegisterUncreatableType<bv::ScanTargetNode>(
        "BinaryValentineNative", 1, 0, "ScanTarget", "");
    qmlRegisterUncreatableType<bv::ScanRunnerNode>(
        "BinaryValentineNative", 1, 0, "ScanRunner", "");
    qmlRegisterUncreatableType<bv::ReportOutputsNode>(
        "BinaryValentineNative", 1, 0, "ReportOutputs", "");
    qmlRegisterUncreatableType<bv::AnalysisProgress>(
        "BinaryValentineNative", 1, 0, "AnalysisProgress", "");

    const QUrl url(u"qrc:/binary-valentine-ui/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
