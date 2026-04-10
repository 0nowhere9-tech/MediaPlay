#include "VideoItem.h"

QString VideoItem::durationString() const
{
    int h = durationSeconds / 3600;
    int m = (durationSeconds % 3600) / 60;
    int s = durationSeconds % 60;

    if (h > 0)
        return QString("%1:%2:%3")
            .arg(h)
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'));
    return QString("%1:%2")
        .arg(m)
        .arg(s, 2, 10, QChar('0'));
}

QString VideoItem::friendlyDate() const
{
    return datePublished.toString("MMM d, yyyy");
}
