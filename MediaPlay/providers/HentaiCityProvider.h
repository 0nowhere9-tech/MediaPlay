#pragma once
#include "BaseProvider.h"

// Provider for HentaiCity — maps HentaiCity-specific JSON fields
// into the normalised VideoItem format.
class HentaiCityProvider : public BaseProvider
{
public:
    QString sourceName() const override { return QStringLiteral("HentaiCity"); }
    bool parseItem(const QJsonObject &json, VideoItem &item) const override;
};
