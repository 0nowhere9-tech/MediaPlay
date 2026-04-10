#pragma once
#include "BaseProvider.h"
#include <memory>
#include <unordered_map>

// Singleton registry that maps source names → providers.
// New providers are registered once at startup via registerProvider().
class ProviderRegistry
{
public:
    static ProviderRegistry &instance();

    // Register a provider (takes ownership).
    void registerProvider(std::unique_ptr<BaseProvider> provider);

    // Look up by source name (case-insensitive). Returns nullptr if unknown.
    const BaseProvider *providerFor(const QString &sourceName) const;

private:
    ProviderRegistry();  // pre-registers built-in providers
    std::unordered_map<QString, std::unique_ptr<BaseProvider>> m_providers;
};
