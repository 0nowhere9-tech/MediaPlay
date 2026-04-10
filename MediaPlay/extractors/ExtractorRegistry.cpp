#include "ExtractorRegistry.h"
#include "HentaiCityExtractor.h"

ExtractorRegistry &ExtractorRegistry::instance()
{
    static ExtractorRegistry reg;
    return reg;
}

ExtractorRegistry::ExtractorRegistry()
{
    // Register all built-in extractors here.
    // Third-party extractors can call registerExtractor() after startup.
    registerExtractor(std::make_unique<HentaiCityExtractor>());
}

void ExtractorRegistry::registerExtractor(std::unique_ptr<BaseExtractor> extractor)
{
    QString key = extractor->sourceName().toLower();
    m_extractors[key] = std::move(extractor);
}

const BaseExtractor *ExtractorRegistry::extractorFor(const QString &sourceName) const
{
    auto it = m_extractors.find(sourceName.toLower());
    return (it != m_extractors.end()) ? it->second.get() : nullptr;
}

QList<QString> ExtractorRegistry::sourceNames() const
{
    QList<QString> names;
    names.reserve(static_cast<int>(m_extractors.size()));
    for (const auto &[key, extractor] : m_extractors)
        names.append(extractor->sourceName());
    return names;
}
