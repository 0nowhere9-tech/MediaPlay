#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "VideoModel.h"
#include "ExtractionManager.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MediaPlayer");
    app.setOrganizationName("MediaPlayer");
    app.setApplicationVersion("1.0");

    // Resolve resources/sources/all_hen.json relative to the binary.
    // When running as ./build/MediaPlayer from the project root, this
    // points to <project>/resources/sources/all_hen.json — a real,
    // writable file that lives in the source tree.
    const QString jsonPath =
        QCoreApplication::applicationDirPath() + u"/../resources/sources/all_hen.json"_s;

    // Create the directory and an empty array if the file doesn't exist yet.
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

    // Append each extracted video to the live model as it arrives.
    QObject::connect(&extractionManager, &ExtractionManager::videoReady,
                     [&defaultModel](const VideoItem &item) {
        defaultModel.appendItem(item);
    });

    // Reload from disk after a run finishes (picks up deduplication).
    QObject::connect(&extractionManager, &ExtractionManager::finished,
                     [&defaultModel, jsonPath](const QString &, int, int) {
        defaultModel.loadFromFile(jsonPath);
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(u"videoModel"_s,        &defaultModel);
    engine.rootContext()->setContextProperty(u"extractionManager"_s, &extractionManager);

    const QUrl url(u"qrc:/qt/qml/mediaplayer/qml/Main.qml"_s);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
