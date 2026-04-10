#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QUrl>
#include <QDir>
#include "VideoModel.h"
#include "ExtractionManager.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MediaPlayer");
    app.setOrganizationName("MediaPlayer");
    app.setApplicationVersion("1.0");

    qmlRegisterType<VideoModel>("mediaplayer", 1, 0, "VideoModel");

    // Writable JSON path — lives in the app's data location so it survives
    // between runs and can be written to at runtime.
    const QString dataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    const QString jsonPath = dataDir + u"/all_videos.json"_s;

    // On first run, seed from the bundled sample so the UI isn't empty.
    if (!QFile::exists(jsonPath)) {
        QFile::copy(u":/qt/qml/mediaplayer/resources/sample/all_videos.json"_s,
                    jsonPath);
        QFile::setPermissions(jsonPath,
            QFile::ReadOwner | QFile::WriteOwner |
            QFile::ReadUser  | QFile::WriteUser);
    }

    QQmlApplicationEngine engine;

    VideoModel defaultModel;
    defaultModel.loadFromFile(jsonPath);
    engine.rootContext()->setContextProperty(u"videoModel"_s, &defaultModel);

    ExtractionManager extractionManager;
    extractionManager.setOutputPath(jsonPath);

    // Append each extracted video to the live model as it arrives.
    QObject::connect(&extractionManager, &ExtractionManager::videoReady,
                     [&defaultModel](const VideoItem &item) {
        defaultModel.appendItem(item);
    });

    // Reload the model from disk after a run finishes (picks up deduplication).
    QObject::connect(&extractionManager, &ExtractionManager::finished,
                     [&defaultModel, jsonPath](const QString &, int, int) {
        defaultModel.loadFromFile(jsonPath);
    });

    engine.rootContext()->setContextProperty(u"extractionManager"_s, &extractionManager);

    const QUrl url(u"qrc:/qt/qml/mediaplayer/qml/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
