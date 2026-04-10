#pragma once
#include "BaseExtractor.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

// HentaiCityExtractor — two-phase extractor for HentaiCity.
//
// Phase 1  fetchListing()
//   Paginates through https://www.hentaicity.com/videos/straight/all-recent-{N}.html
//   collecting every unique video page URL.  Stops at the last page number
//   found in the pagination links (or kMaxPages, whichever is smaller).
//
// Phase 2  fetchVideoData(pageUrl)
//   Fetches each video page and extracts metadata from:
//     - The schema.org JSON-LD <script> block (title, author, thumbnail,
//       uploadDate, duration, contentUrl, views)
//     - Regex scraping for like-ratio %, vote count, and category tags.

class HentaiCityExtractor : public BaseExtractor
{
public:
    QString sourceName() const override { return QStringLiteral("HentaiCity"); }

    QList<QString> fetchListing() const override;
    bool fetchVideoData(const QString &pageUrl, VideoItem &item) const override;

private:
    static constexpr const char *kListingBase =
        "https://www.hentaicity.com/videos/straight/all-recent-%1.html";
    static constexpr const char *kUserAgent =
        "Mozilla/5.0 (X11; Linux x86_64; rv:124.0) Gecko/20100101 Firefox/124.0";
    static constexpr int kMaxPages = 135;

    // Blocking HTTP GET — returns empty string on failure.
    QString fetch(const QString &url) const;

    // Parse helpers
    int  lastPageFromHtml(const QString &html)           const;
    QList<QString> videoUrlsFromHtml(const QString &html) const;
};
