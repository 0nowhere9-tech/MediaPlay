#pragma once
#include <QString>
#include <QList>
#include "VideoItem.h"

// Reads a JSON array from a file path and produces a list of VideoItems.
// Each object in the array is dispatched to the matching provider via
// ProviderRegistry based on the "source" field.
//
// Unknown sources are skipped with a warning.
class JsonLoader
{
public:
    // Returns loaded items; populates errorMessage on failure.
    static QList<VideoItem> load(const QString &filePath,
                                 QString       &errorMessage);
};
