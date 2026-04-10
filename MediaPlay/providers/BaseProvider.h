#pragma once
#include <QJsonObject>
#include <QList>
#include <QString>
#include "VideoItem.h"

// Abstract base for all video source providers.
//
// To add a new source (e.g. "Pornhub", "RedTube"):
//   1. Subclass BaseProvider
//   2. Implement sourceName() and parseItem()
//   3. Register in ProviderRegistry
//
// The provider is responsible for mapping raw JSON fields from its
// specific site into the normalised VideoItem structure.
class BaseProvider
{
public:
    virtual ~BaseProvider() = default;

    // Unique name matching the "source" field in JSON (case-insensitive).
    virtual QString sourceName() const = 0;

    // Parse a single JSON object into a VideoItem.
    // Returns false and leaves item unchanged on parse failure.
    virtual bool parseItem(const QJsonObject &json, VideoItem &item) const = 0;

protected:
    // Shared helper: safely read a string from a JSON object.
    static QString str(const QJsonObject &o, const QString &key,
                       const QString &fallback = {})
    {
        return o.value(key).toString(fallback);
    }

    static int intVal(const QJsonObject &o, const QString &key, int fallback = 0)
    {
        auto v = o.value(key);
        if (v.isString()) return v.toString().toInt();
        return v.toInt(fallback);
    }
};
