#pragma once
#include <QJsonObject>
#include <QObject>
#include <QString>

// SettingsManager — thin C++ bridge between QML SettingsStore and a JSON file.
//
// Exposed to QML as the context property "settingsManager".
//
// Usage from QML (SettingsStore.qml):
//
//   // On startup — read a value with a fallback default
//   property real videosScrollSpeed: settingsManager.get("videos/scrollSpeed", 3.0)
//
//   // Whenever a value changes — stage it (does NOT write to disk yet)
//   onVideosScrollSpeedChanged: settingsManager.set("videos/scrollSpeed", videosScrollSpeed)
//
// Usage from C++ (main.cpp):
//
//   QObject::connect(&app, &QGuiApplication::aboutToQuit,
//                    &settingsManager, &SettingsManager::save);
//
// The JSON file lives at <binary>/../resources/settings.json so it stays
// next to the source tree when running via ./build/MediaPlayer.

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(const QString &filePath, QObject *parent = nullptr);

    // Read a value by slash-separated key (e.g. "videos/scrollSpeed").
    // Returns fallback if the key doesn't exist.
    Q_INVOKABLE QVariant get(const QString &key, const QVariant &fallback = {}) const;

    // Stage a value. Stored in memory; written to disk only on save().
    Q_INVOKABLE void set(const QString &key, const QVariant &value);

public slots:
    // Write the current in-memory state to disk.
    void save();

private:
    void load();

    // Resolve "section/key" into the nested JSON object structure.
    static QVariant readNested(const QJsonObject &root, const QString &key);
    static void     writeNested(QJsonObject &root,      const QString &key, const QVariant &value);

    QString     m_filePath;
    QJsonObject m_data;   // in-memory store
};
