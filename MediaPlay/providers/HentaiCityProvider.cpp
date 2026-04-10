#include "HentaiCityProvider.h"
#include <QDateTime>

bool HentaiCityProvider::parseItem(const QJsonObject &json, VideoItem &item) const
{
    item.source       = sourceName();
    item.pageUrl      = str(json, "pageUrl");
    item.videoUrl     = str(json, "videoUrl");
    item.thumbnailUrl = str(json, "thumbnailUrl");
    item.title        = str(json, "title");
    item.description  = str(json, "description");
    item.author       = str(json, "author");

    // Tags are comma-separated in this source
    const QString rawTags = str(json, "tags");
    if (!rawTags.isEmpty())
        item.tags = rawTags.split(',', Qt::SkipEmptyParts);

    item.durationSeconds = intVal(json, "length");
    item.likes           = intVal(json, "likes");
    item.dislikes        = intVal(json, "dislikes");
    item.views           = intVal(json, "views");
    item.ratio           = intVal(json, "ratio");

    // datePublished: accept ISO 8601 (from extractor) or unix timestamp string (legacy)
    const QString dateStr = str(json, "datePublished");
    QDateTime dt = QDateTime::fromString(dateStr, Qt::ISODate);
    if (!dt.isValid()) {
        qint64 ts = dateStr.toLongLong();
        if (ts > 0)
            dt = QDateTime::fromSecsSinceEpoch(ts);
    }
    item.datePublished = dt;

    return !item.videoUrl.isEmpty();
}
