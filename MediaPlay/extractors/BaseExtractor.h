#pragma once
#include <QObject>
#include <QList>
#include <QString>
#include "VideoItem.h"

// BaseExtractor — abstract interface for all video-source extractors.
//
// An extractor is responsible for two network-facing phases:
//
//   Phase 1 — fetchListing():
//     Hit the source's index/browse endpoint and return a list of
//     per-video page URLs.  The list may span multiple pages; the
//     implementation decides how many pages to fetch.
//
//   Phase 2 — fetchVideoData(pageUrl):
//     Given a single video page URL, scrape / call the API and return
//     a fully-populated VideoItem.  Returns false on failure.
//
// Both operations are synchronous within the worker thread that
// ExtractionManager spins up; do not call them on the GUI thread.
//
// To add a new source:
//   1. Subclass BaseExtractor and implement sourceName(),
//      fetchListing(), and fetchVideoData().
//   2. Register an instance in ExtractorRegistry::ExtractorRegistry().

class BaseExtractor
{
public:
    virtual ~BaseExtractor() = default;

    // Unique source identifier — must match BaseProvider::sourceName()
    // for the corresponding provider (case-insensitive).
    virtual QString sourceName() const = 0;

    // Phase 1: return the page URLs for every video on the source.
    // On network failure, return an empty list (log internally).
    virtual QList<QString> fetchListing() const = 0;

    // Phase 2: scrape a single video page into a VideoItem.
    // Returns true on success, false if the page cannot be parsed.
    virtual bool fetchVideoData(const QString &pageUrl, VideoItem &item) const = 0;
};
