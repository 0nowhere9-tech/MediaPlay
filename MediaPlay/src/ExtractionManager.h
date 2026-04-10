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
// Phase 1 — "listing":  fetches the index pages to collect video page URLs.
// Phase 2 — "fetching": scrapes each video page; skips already-known URLs.
//
// QML-exposed properties:
//   running     bool   — true while a run is active
//   progress    int    — 0-100 within the current phase
//   phase       string — "listing" | "fetching" | ""
//   statusText  string — human-readable one-liner for the UI
//   eta         string — estimated time remaining (phase 2 only), e.g. "3m 12s"

class ExtractionManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool    running    READ isRunning        NOTIFY runningChanged)
    Q_PROPERTY(int     progress   READ currentProgress  NOTIFY progressChanged)
    Q_PROPERTY(QString phase      READ currentPhase     NOTIFY phaseChanged)
    Q_PROPERTY(QString statusText READ currentStatus    NOTIFY statusChanged)
    Q_PROPERTY(QString eta        READ currentEta       NOTIFY etaChanged)

public:
    explicit ExtractionManager(QObject *parent = nullptr);

    bool    isRunning()       const { return m_running; }
    int     currentProgress() const { return m_progress; }
    QString currentPhase()    const { return m_phase; }
    QString currentStatus()   const { return m_statusText; }
    QString currentEta()      const { return m_eta; }

    void setOutputPath(const QString &path) { m_outputPath = path; }

public slots:
    void refresh(const QString &sourceName);
    void cancel();

signals:
    void videoReady(const VideoItem &item);
    void finished(const QString &sourceName, int total, int success);
    void error(const QString &sourceName, const QString &message);
    void runningChanged();
    void progressChanged();
    void phaseChanged();
    void statusChanged();
    void etaChanged();

private:
    void runExtraction(const QString &sourceName);
    void appendItemsToJson(const QList<VideoItem> &items) const;

    void setRunning(bool v);
    void setProgress(int v);
    void setPhase(const QString &v);
    void setStatus(const QString &v);
    void setEta(const QString &v);

    static QString formatEta(qint64 seconds);

    bool           m_running   = false;
    bool           m_cancelled = false;
    int            m_progress  = 0;
    QString        m_phase;
    QString        m_statusText;
    QString        m_eta;
    QFuture<void>  m_future;
    QString        m_outputPath;
    mutable QMutex m_writeMutex;
};
