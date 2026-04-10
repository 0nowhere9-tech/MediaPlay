#pragma once
#include <QString>
#include <QStringList>
#include <QDateTime>

// Pure data class representing a single video entry.
// Populated by a provider from raw JSON and exposed to QML via VideoModel.
class VideoItem
{
public:
    VideoItem() = default;

    // --- Core identity ---
    QString source;       // e.g. "HentaiCity"
    QString pageUrl;
    QString videoUrl;
    QString thumbnailUrl;
    QString title;
    QString description;
    QString author;

    // --- Metadata ---
    QStringList tags;
    QDateTime   datePublished;
    int         durationSeconds = 0;

    // --- Stats ---
    int likes    = 0;
    int dislikes = 0;
    int views    = 0;
    int ratio    = 0;   // like ratio 0-100

    // Convenience
    QString durationString() const;
    QString friendlyDate()   const;
};
