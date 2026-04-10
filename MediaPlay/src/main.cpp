#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "VideoModel.h"
#include "ExtractionManager.h"
#include "SettingsManager.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MediaPlayer");
    app.setOrganizationName("MediaPlayer");
    app.setApplicationVersion("1.0");

    const QString resourcesDir =
        QCoreApplication::applicationDirPath() + u"/../resources"_s;

    // ── Settings ──────────────────────────────────────────────────
    const QString settingsPath = resourcesDir + u"/settings.json"_s;
    QDir().mkpath(QFileInfo(settingsPath).absolutePath());

    SettingsManager settingsManager(settingsPath);

    // ── Video JSON ────────────────────────────────────────────────
    const QString jsonPath = resourcesDir + u"/sources/all_hen.json"_s;
    QDir().mkpath(QFileInfo(jsonPath).absolutePath());
    if (!QFile::exists(jsonPath)) {
        QFile f(jsonPath);
        if (f.open(QIODevice::WriteOnly))
            f.write("[]");
    }

    VideoModel defaultModel;
    defaultModel.loadFromFile(jsonPath);

    ExtractionManager extractionManager;
    extractionManager.setOutputPath(jsonPath);

    QObject::connect(&extractionManager, &ExtractionManager::videoReady,
                     [&defaultModel](const VideoItem &item) {
        defaultModel.appendItem(item);
    });
    QObject::connect(&extractionManager, &ExtractionManager::finished,
                     [&defaultModel, jsonPath](const QString &, int, int) {
        defaultModel.loadFromFile(jsonPath);
    });

    // ── Save settings when the app closes ────────────────────────
    QObject::connect(&app, &QGuiApplication::aboutToQuit,
                     &settingsManager, &SettingsManager::save);

    // ── QML engine ────────────────────────────────────────────────
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(u"videoModel"_s,        &defaultModel);
    engine.rootContext()->setContextProperty(u"extractionManager"_s, &extractionManager);
    engine.rootContext()->setContextProperty(u"settingsManager"_s,   &settingsManager);

    const QUrl url(u"qrc:/qt/qml/mediaplayer/qml/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
