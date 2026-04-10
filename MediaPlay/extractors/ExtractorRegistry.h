#pragma once
#include "BaseExtractor.h"
#include <memory>
#include <unordered_map>

// ExtractorRegistry — singleton that maps source names to extractors.
//
// Mirrors ProviderRegistry but for the network-extraction layer.
// Register new extractors in the constructor body of ExtractorRegistry.cpp.

class ExtractorRegistry
{
public:
    static ExtractorRegistry &instance();

    // Register an extractor (takes ownership).
    void registerExtractor(std::unique_ptr<BaseExtractor> extractor);

    // Look up by source name (case-insensitive). Returns nullptr if unknown.
    const BaseExtractor *extractorFor(const QString &sourceName) const;

    // All registered source names, for UI enumeration.
    QList<QString> sourceNames() const;

private:
    ExtractorRegistry();   // pre-registers built-in extractors
    std::unordered_map<QString, std::unique_ptr<BaseExtractor>> m_extractors;
};
