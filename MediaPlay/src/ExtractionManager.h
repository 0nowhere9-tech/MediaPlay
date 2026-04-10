#pragma once
#include <QFuture>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include "VideoItem.h"

// ExtractionManager — orchestrates a full two-phase extraction run
// for a single source, on a background thread.
//
// Usage (from QML via a context property, or C++):
//
//   manager->refresh("HentaiCity");
//
// The manager will:
//   1. Look up the extractor in ExtractorRegistry.
//   2. Call fetchListing() to get all video page URLs.
//   3. Call fetchVideoData() for each URL, emitting videoReady()
//      as each item completes so the UI can update incrementally.
//   4. Append all successful items to the JSON file.
//   5. Emit finished() when all items are done, or error() on failure.
//
// All network work runs in a QThread; signals are cross-thread safe.

class ExtractionManager : public QObject
{
    Q_OBJECT

    // Exposed to QML so SourcesPage can show a spinner / progress bar.
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(int  progress READ currentProgress NOTIFY progressChanged)

public:
    explicit ExtractionManager(QObject *parent = nullptr);

    bool isRunning()      const { return m_running; }
    int  currentProgress() const { return m_progress; }

    // Path to the JSON file where extracted items are persisted.
    // Defaults to the bundled sample file; override before calling refresh().
    void setOutputPath(const QString &path) { m_outputPath = path; }

public slots:
    // Start a refresh for the named source (lowercase, e.g. "hentaicity").
    // Safe to call from QML. Ignored if a run is already in progress.
    void refresh(const QString &sourceName);

    // Cancel the running extraction after the current video finishes.
    void cancel();

signals:
    void videoReady(const VideoItem &item);
    void finished(const QString &sourceName, int total, int success);
    void error(const QString &sourceName, const QString &message);
    void runningChanged();
    void progressChanged();

private:
    void runExtraction(const QString &sourceName);
    void appendItemsToJson(const QList<VideoItem> &items) const;
    void setRunning(bool v);
    void setProgress(int v);

    bool           m_running   = false;
    bool           m_cancelled = false;
    int            m_progress  = 0;
    QFuture<void>  m_future;
    QString        m_outputPath;
    mutable QMutex m_writeMutex;
};
