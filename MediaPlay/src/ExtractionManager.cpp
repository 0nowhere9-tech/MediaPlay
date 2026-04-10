#include "ExtractionManager.h"
#include "ExtractorRegistry.h"
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QDebug>

ExtractionManager::ExtractionManager(QObject *parent)
    : QObject(parent)
    , m_outputPath(QStringLiteral("resources/sources/all_hen.json"))
{}

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
    setPhase(QString());
    setStatus(QString());
    setEta(QString());
    setRunning(true);
    m_future = QtConcurrent::run([this, sourceName]() { runExtraction(sourceName); });
}

void ExtractionManager::cancel() { m_cancelled = true; }

void ExtractionManager::setRunning(bool v)  { if (m_running    == v) return; m_running    = v; emit runningChanged();  }
void ExtractionManager::setProgress(int v)  { if (m_progress   == v) return; m_progress   = v; emit progressChanged(); }
void ExtractionManager::setPhase(const QString &v) { if (m_phase      == v) return; m_phase      = v; emit phaseChanged();    }
void ExtractionManager::setStatus(const QString &v){ if (m_statusText == v) return; m_statusText = v; emit statusChanged();   }
void ExtractionManager::setEta(const QString &v)   { if (m_eta        == v) return; m_eta        = v; emit etaChanged();      }

QString ExtractionManager::formatEta(qint64 seconds)
{
    if (seconds <= 0) return QStringLiteral("calculating…");
    const qint64 h = seconds / 3600;
    const qint64 m = (seconds % 3600) / 60;
    const qint64 s = seconds % 60;
    if (h > 0) return QStringLiteral("%1h %2m").arg(h).arg(m);
    if (m > 0) return QStringLiteral("%1m %2s").arg(m).arg(s);
    return QStringLiteral("%1s").arg(s);
}

void ExtractionManager::runExtraction(const QString &sourceName)
{
    const BaseExtractor *extractor = ExtractorRegistry::instance().extractorFor(sourceName);
    if (!extractor) {
        emit error(sourceName, QStringLiteral("Extractor disappeared during run"));
        setRunning(false);
        return;
    }

    // ── Phase 1: listing ─────────────────────────────────────────
    setPhase(QStringLiteral("listing"));
    setProgress(0);
    setStatus(QStringLiteral("Fetching video list…"));
    setEta(QString());

    qDebug() << "ExtractionManager: starting listing for" << sourceName;
    const QList<QString> pageUrls = extractor->fetchListing();

    if (pageUrls.isEmpty()) {
        emit error(sourceName, QStringLiteral("Listing returned no URLs — check network or extractor"));
        setPhase(QString()); setStatus(QString()); setRunning(false);
        return;
    }

    setProgress(100);
    setStatus(QStringLiteral("Got %1 URLs").arg(pageUrls.size()));

    // ── Load already-known URLs ───────────────────────────────────
    QSet<QString> knownUrls;
    {
        QMutexLocker lock(&m_writeMutex);
        QFile f(m_outputPath);
        if (f.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            if (doc.isArray())
                for (const QJsonValue &v : doc.array())
                    if (v.isObject())
                        knownUrls.insert(v.toObject().value(QStringLiteral("pageUrl")).toString());
        }
    }

    const int newCount = static_cast<int>(
        std::count_if(pageUrls.begin(), pageUrls.end(),
                      [&](const QString &u){ return !knownUrls.contains(u); }));

    qDebug() << "ExtractionManager:" << knownUrls.size() << "already known,"
             << newCount << "new to fetch";

    // ── Phase 2: fetch video data ─────────────────────────────────
    setPhase(QStringLiteral("fetching"));
    setProgress(0);
    setEta(QStringLiteral("calculating…"));

    const int total   = pageUrls.size();
    int fetched = 0, success = 0, skipped = 0;
    QList<VideoItem> batch;
    batch.reserve(newCount);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < total; ++i) {
        if (m_cancelled) {
            qDebug() << "ExtractionManager: cancelled after" << i << "of" << total;
            break;
        }

        const QString &url = pageUrls.at(i);

        if (knownUrls.contains(url)) {
            ++skipped;
            setProgress((i + 1) * 100 / total);
            continue;
        }

        VideoItem item;
        if (extractor->fetchVideoData(url, item)) {
            ++success;
            batch.append(item);
            emit videoReady(item);
        } else {
            qWarning() << "ExtractionManager: fetchVideoData failed for" << url;
        }

        ++fetched;
        setProgress((i + 1) * 100 / total);
        setStatus(QStringLiteral("%1 / %2 new  ·  %3 skipped")
                  .arg(success).arg(newCount).arg(skipped));

        if (fetched > 0 && newCount > success) {
            const qint64 elapsedMs  = timer.elapsed();
            const double msPerFetch = static_cast<double>(elapsedMs) / fetched;
            const int    remaining  = newCount - success;
            setEta(formatEta(static_cast<qint64>(msPerFetch * remaining / 1000.0)));
        }
    }

    if (!batch.isEmpty())
        appendItemsToJson(batch);

    setPhase(QString()); setStatus(QString()); setEta(QString());
    qDebug() << "ExtractionManager: done — fetched" << success << "new, skipped" << skipped;
    emit finished(sourceName, total, success);
    setRunning(false);
}

void ExtractionManager::appendItemsToJson(const QList<VideoItem> &items) const
{
    QMutexLocker lock(&m_writeMutex);

    QJsonArray existing;
    QFile f(m_outputPath);
    if (f.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        if (doc.isArray()) existing = doc.array();
        f.close();
    }

    QSet<QString> seen;
    for (const QJsonValue &v : existing)
        if (v.isObject())
            seen.insert(v.toObject().value(QStringLiteral("pageUrl")).toString());

    int added = 0;
    for (const VideoItem &item : items) {
        if (seen.contains(item.pageUrl)) continue;
        QJsonObject obj;
        obj[QStringLiteral("source")]        = item.source;
        obj[QStringLiteral("pageUrl")]       = item.pageUrl;
        obj[QStringLiteral("videoUrl")]      = item.videoUrl;
        obj[QStringLiteral("thumbnailUrl")]  = item.thumbnailUrl;
        obj[QStringLiteral("title")]         = item.title;
        obj[QStringLiteral("description")]   = item.description;
        obj[QStringLiteral("author")]        = item.author;
        obj[QStringLiteral("tags")]          = item.tags.join(QLatin1Char(','));
        obj[QStringLiteral("datePublished")] = item.datePublished.toString(Qt::ISODate);
        obj[QStringLiteral("length")]        = item.durationSeconds;
        obj[QStringLiteral("likes")]         = item.likes;
        obj[QStringLiteral("dislikes")]      = item.dislikes;
        obj[QStringLiteral("views")]         = item.views;
        obj[QStringLiteral("ratio")]         = item.ratio;
        existing.append(obj);
        seen.insert(item.pageUrl);
        ++added;
    }

    if (added == 0) { qDebug() << "ExtractionManager: no new items to write"; return; }

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "ExtractionManager: cannot write to" << m_outputPath;
        return;
    }
    f.write(QJsonDocument(existing).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "ExtractionManager: wrote" << added << "new items to" << m_outputPath;
}
