#include "HentaiCityExtractor.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QRegularExpression>
#include <QSet>
#include <QDebug>

// ---------------------------------------------------------------------------
// Network helper — synchronous blocking GET on the calling thread.
// ExtractionManager always calls us from a worker thread, so spinning a local
// QEventLoop here is safe (no GUI thread involvement).
// ---------------------------------------------------------------------------
QString HentaiCityExtractor::fetch(const QString &url) const
{
    QNetworkAccessManager nam;
    QNetworkRequest req{QUrl(url)};
    req.setRawHeader("User-Agent", kUserAgent);
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    req.setRawHeader("Accept-Language", "en-US,en;q=0.5");

    QNetworkReply *reply = nam.get(req);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "HentaiCityExtractor: fetch error for" << url
                   << "—" << reply->errorString();
        reply->deleteLater();
        return {};
    }

    QString html = QString::fromUtf8(reply->readAll());
    reply->deleteLater();
    return html;
}

// ---------------------------------------------------------------------------
// Listing helpers
// ---------------------------------------------------------------------------

// Find the highest page number from pagination links like:
//   /videos/straight/all-recent-135.html
int HentaiCityExtractor::lastPageFromHtml(const QString &html) const
{
    static const QRegularExpression re(
        QStringLiteral(R"(all-recent-(\d+)\.html)"));

    int last = 1;
    auto it = re.globalMatch(html);
    while (it.hasNext()) {
        int n = it.next().captured(1).toInt();
        if (n > last) last = n;
    }
    return qMin(last, kMaxPages);
}

// Extract unique direct video URLs from a listing page.
// Links are wrapped in a click-tracker: /click/1-N/video/slug.html
// We strip the tracker prefix to get the canonical URL.
QList<QString> HentaiCityExtractor::videoUrlsFromHtml(const QString &html) const
{
  static const QRegularExpression re(
      QStringLiteral(R"REGEX(href="(https://www\.hentaicity\.com/(?:click/\d+-\d+/)?video/[^"]+)")REGEX")
      );

  QSet<QString> seen;
  QList<QString> urls;

    auto it = re.globalMatch(html);
    while (it.hasNext()) {
        QString url = it.next().captured(1);
        // Strip click-tracker prefix
        url.replace(QRegularExpression(QStringLiteral(R"(/click/\d+-\d+/)")),
                    QStringLiteral("/"));
        if (!seen.contains(url)) {
            seen.insert(url);
            urls.append(url);
        }
    }
    return urls;
}

// ---------------------------------------------------------------------------
// Phase 1: fetchListing
// ---------------------------------------------------------------------------
QList<QString> HentaiCityExtractor::fetchListing(ListingProgressCb progressCb) const
{
    QList<QString> allUrls;

    // Fetch page 1 first to discover how many pages exist.
    QString page1Html = fetch(QString::fromLatin1(kListingBase).arg(1));
    if (page1Html.isEmpty()) {
        qWarning() << "HentaiCityExtractor: failed to fetch page 1";
        return {};
    }

    allUrls.append(videoUrlsFromHtml(page1Html));
    int lastPage = lastPageFromHtml(page1Html);
    qDebug() << "HentaiCityExtractor: found" << lastPage << "pages";

    // Report progress after page 1 — now we know the total
    if (progressCb) progressCb(1, lastPage);

    for (int page = 2; page <= lastPage; ++page) {
        QString url  = QString::fromLatin1(kListingBase).arg(page);
        QString html = fetch(url);
        if (html.isEmpty()) {
            qWarning() << "HentaiCityExtractor: failed to fetch page" << page << "— stopping";
            break;
        }
        allUrls.append(videoUrlsFromHtml(html));
        qDebug() << "HentaiCityExtractor: page" << page << "—" << allUrls.size() << "URLs so far";

        if (progressCb) progressCb(page, lastPage);
    }

    qDebug() << "HentaiCityExtractor: listing complete —" << allUrls.size() << "videos";
    return allUrls;
}

// ---------------------------------------------------------------------------
// Phase 2: fetchVideoData
// Parse schema.org JSON-LD for the bulk of metadata, then regex-scrape the
// remaining fields (ratio, votes, tags) that aren't in the structured data.
// ---------------------------------------------------------------------------
bool HentaiCityExtractor::fetchVideoData(const QString &pageUrl, VideoItem &item) const
{
    QString html = fetch(pageUrl);
    if (html.isEmpty())
        return false;

    // --- Schema.org JSON-LD block ---
    // The site embeds a VideoObject inside a bare <script> tag (no type attr).
    static const QRegularExpression jsonRe(
        QStringLiteral(R"(<script[^>]*>\s*\{[^<]*"@type"\s*:\s*"VideoObject"[^<]*\}[^<]*</script>)"),
        QRegularExpression::DotMatchesEverythingOption);

    auto jsonMatch = jsonRe.match(html);
    if (!jsonMatch.hasMatch()) {
        qWarning() << "HentaiCityExtractor: no JSON-LD found for" << pageUrl;
        return false;
    }

    // Extract the raw JSON object from the script tag
    static const QRegularExpression jsonBody(
        QStringLiteral(R"(\{.*\})"),
        QRegularExpression::DotMatchesEverythingOption);
    auto bodyMatch = jsonBody.match(jsonMatch.captured(0));
    if (!bodyMatch.hasMatch())
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(
        bodyMatch.captured(0).toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "HentaiCityExtractor: JSON parse error for" << pageUrl
                   << "—" << err.errorString();
        return false;
    }

    QJsonObject obj = doc.object();

    item.source       = sourceName();
    item.pageUrl      = pageUrl;
    item.title        = obj.value(QStringLiteral("name")).toString();
    item.author       = obj.value(QStringLiteral("author")).toString();
    item.thumbnailUrl = obj.value(QStringLiteral("thumbnailUrl")).toString();
    item.videoUrl     = obj.value(QStringLiteral("contentUrl")).toString();

    // description — strip the "Watch X. Uploaded by Y to Hentai City. " boilerplate
    QString desc = obj.value(QStringLiteral("description")).toString();
    static const QRegularExpression descBoiler(
        QStringLiteral(R"(^Watch .+?\. Uploaded by .+? to Hentai City\.\s*)"));
    desc.remove(descBoiler);
    item.description = desc.trimmed();

    // uploadDate: ISO 8601 e.g. "2026-04-07T00:00:00-07:00"
    item.datePublished = QDateTime::fromString(
        obj.value(QStringLiteral("uploadDate")).toString(), Qt::ISODate);

    // duration: ISO 8601 duration e.g. "PT00H13M04S"
    QString dur = obj.value(QStringLiteral("duration")).toString();
    static const QRegularExpression durRe(
        QStringLiteral(R"(PT(\d+)H(\d+)M(\d+)S)"));
    auto durMatch = durRe.match(dur);
    if (durMatch.hasMatch()) {
        item.durationSeconds = durMatch.captured(1).toInt() * 3600
                             + durMatch.captured(2).toInt() * 60
                             + durMatch.captured(3).toInt();
    }

    // views from interactionStatistic
    QJsonObject stat = obj.value(QStringLiteral("interactionStatistic")).toObject();
    item.views = stat.value(QStringLiteral("userInteractionCount")).toInt();

    // --- Regex-scraped fields not in JSON-LD ---

    // Like ratio %: <div>60%</div> immediately after the like SVG block
    static const QRegularExpression ratioRe(
        QStringLiteral(R"(<div>(\d+)%</div>)"));
    auto ratioMatch = ratioRe.match(html);
    item.ratio = ratioMatch.hasMatch() ? ratioMatch.captured(1).toInt() : 0;

    // Vote count: "168 Votes"
    static const QRegularExpression votesRe(
        QStringLiteral(R"((\d+)\s*Votes)"));
    auto votesMatch = votesRe.match(html);
    // Store votes in likes field (site doesn't expose separate like/dislike counts)
    item.likes = votesMatch.hasMatch() ? votesMatch.captured(1).toInt() : 0;

    // Tags: <a href="/...category.../...">tag name</a>
    static const QRegularExpression tagRe(
        QStringLiteral(R"(<a[^>]+href="[^"]*(?:tag|categor)[^"]*"[^>]*>([^<]+)</a>)"),
        QRegularExpression::CaseInsensitiveOption);
    auto tagIt = tagRe.globalMatch(html);
    while (tagIt.hasNext())
        item.tags.append(tagIt.next().captured(1).trimmed());
    item.tags.removeDuplicates();

    if (item.title.isEmpty() || item.videoUrl.isEmpty()) {
        qWarning() << "HentaiCityExtractor: incomplete data for" << pageUrl;
        return false;
    }

    return true;
}
