#include "ProviderRegistry.h"
#include "HentaiCityProvider.h"

ProviderRegistry &ProviderRegistry::instance()
{
    static ProviderRegistry reg;
    return reg;
}

ProviderRegistry::ProviderRegistry()
{
    // Register all built-in providers here.
    // Third-party / plugin providers can call registerProvider() later.
    registerProvider(std::make_unique<HentaiCityProvider>());
}

void ProviderRegistry::registerProvider(std::unique_ptr<BaseProvider> provider)
{
    QString key = provider->sourceName().toLower();
    m_providers[key] = std::move(provider);
}

const BaseProvider *ProviderRegistry::providerFor(const QString &sourceName) const
{
    auto it = m_providers.find(sourceName.toLower());
    return (it != m_providers.end()) ? it->second.get() : nullptr;
}
