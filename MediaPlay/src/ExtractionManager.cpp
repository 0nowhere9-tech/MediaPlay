#include "ExtractionManager.h"
#include "ExtractorRegistry.h"
#include <QtConcurrent/QtConcurrent>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QDateTime>
#include <QDebug>

ExtractionManager::ExtractionManager(QObject *parent)
    : QObject(parent)
    , m_outputPath(QStringLiteral(":/qt/qml/mediaplayer/resources/sample/all_videos.json"))
{}

// ---------------------------------------------------------------------------
// Public slots
// ---------------------------------------------------------------------------

void ExtractionManager::refresh(const QString &sourceName)
{
    if (m_running) {
        qWarning() << "ExtractionManager: already running — ignoring refresh(" << sourceName << ")";
        return;
    }

    if (!ExtractorRegistry::instance().extractorFor(sourceName)) {
        emit error(sourceName,
                   QStringLiteral("No extractor registered for source: %1").arg(sourceName));
        return;
    }

    m_cancelled = false;
    setProgress(0);
    setRunning(true);

    m_future = QtConcurrent::run([this, sourceName]() {
        runExtraction(sourceName);
    });
}

void ExtractionManager::cancel()
{
    m_cancelled = true;
}

// ---------------------------------------------------------------------------
// Private — runs on worker thread
// ---------------------------------------------------------------------------

void ExtractionManager::setRunning(bool v)
{
    if (m_running == v) return;
    m_running = v;
    emit runningChanged();
}

void ExtractionManager::setProgress(int v)
{
    if (m_progress == v) return;
    m_progress = v;
    emit progressChanged();
}

void ExtractionManager::runExtraction(const QString &sourceName)
{
    const BaseExtractor *extractor =
        ExtractorRegistry::instance().extractorFor(sourceName);
    if (!extractor) {
        emit error(sourceName, QStringLiteral("Extractor disappeared during run"));
        setRunning(false);
        return;
    }

    // --- Phase 1: fetch the listing ---
    qDebug() << "ExtractionManager: starting listing for" << sourceName;
    const QList<QString> pageUrls = extractor->fetchListing();

    if (pageUrls.isEmpty()) {
        emit error(sourceName,
                   QStringLiteral("Listing returned no URLs — check network or extractor"));
        setRunning(false);
        return;
    }

    // --- Phase 2: fetch each video page ---
    const int total = pageUrls.size();
    int success = 0;
    QList<VideoItem> batch;
    batch.reserve(total);

    for (int i = 0; i < total; ++i) {
        if (m_cancelled) {
            qDebug() << "ExtractionManager: cancelled after" << i << "of" << total;
            break;
        }

        VideoItem item;
        if (extractor->fetchVideoData(pageUrls.at(i), item)) {
            ++success;
            batch.append(item);
            emit videoReady(item);
        } else {
            qWarning() << "ExtractionManager: fetchVideoData failed for" << pageUrls.at(i);
        }

        setProgress((i + 1) * 100 / total);
    }

    // --- Persist to JSON ---
    if (!batch.isEmpty())
        appendItemsToJson(batch);

    emit finished(sourceName, total, success);
    setRunning(false);
}

// ---------------------------------------------------------------------------
// JSON persistence
// Reads the existing file, merges new items (deduplicating by pageUrl),
// then writes the result back.
// ---------------------------------------------------------------------------
void ExtractionManager::appendItemsToJson(const QList<VideoItem> &items) const
{
    QMutexLocker lock(&m_writeMutex);

    // Build set of existing pageUrls so we don't duplicate
    QJsonArray existing;
    QFile f(m_outputPath);
    if (f.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        if (doc.isArray()) existing = doc.array();
        f.close();
    }

    QSet<QString> seen;
    for (const QJsonValue &v : existing) {
        if (v.isObject())
            seen.insert(v.toObject().value(QStringLiteral("pageUrl")).toString());
    }

    // Serialise new items
    int added = 0;
    for (const VideoItem &item : items) {
        if (seen.contains(item.pageUrl)) continue;

        QJsonObject obj;
        obj[QStringLiteral("source")]       = item.source;
        obj[QStringLiteral("pageUrl")]      = item.pageUrl;
        obj[QStringLiteral("videoUrl")]     = item.videoUrl;
        obj[QStringLiteral("thumbnailUrl")] = item.thumbnailUrl;
        obj[QStringLiteral("title")]        = item.title;
        obj[QStringLiteral("description")]  = item.description;
        obj[QStringLiteral("author")]       = item.author;
        obj[QStringLiteral("tags")]         = item.tags.join(QLatin1Char(','));
        obj[QStringLiteral("datePublished")]= item.datePublished.toString(Qt::ISODate);
        obj[QStringLiteral("length")]       = item.durationSeconds;
        obj[QStringLiteral("likes")]        = item.likes;
        obj[QStringLiteral("dislikes")]     = item.dislikes;
        obj[QStringLiteral("views")]        = item.views;
        obj[QStringLiteral("ratio")]        = item.ratio;

        existing.append(obj);
        seen.insert(item.pageUrl);
        ++added;
    }

    if (added == 0) {
        qDebug() << "ExtractionManager: no new items to write";
        return;
    }

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "ExtractionManager: cannot write to" << m_outputPath;
        return;
    }

    f.write(QJsonDocument(existing).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "ExtractionManager: wrote" << added << "new items to" << m_outputPath;
}
