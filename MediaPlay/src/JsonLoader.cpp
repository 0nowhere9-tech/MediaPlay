#include "JsonLoader.h"
#include "ProviderRegistry.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

QList<VideoItem> JsonLoader::load(const QString &filePath, QString &errorMessage)
{
    QList<VideoItem> items;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = QStringLiteral("Cannot open file: %1").arg(filePath);
        return items;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        errorMessage = QStringLiteral("JSON parse error: %1").arg(parseError.errorString());
        return items;
    }

    if (!doc.isArray()) {
        errorMessage = QStringLiteral("Expected a JSON array at root");
        return items;
    }

    const auto &registry = ProviderRegistry::instance();
    const QJsonArray arr = doc.array();

    for (const QJsonValue &val : arr) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        QString source = obj.value(QStringLiteral("source")).toString();
        const BaseProvider *provider = registry.providerFor(source);

        if (!provider) {
            qWarning() << "No provider registered for source:" << source << "— skipping item.";
            continue;
        }

        VideoItem item;
        if (provider->parseItem(obj, item))
            items.append(item);
        else
            qWarning() << "Provider" << source << "failed to parse item — skipping.";
    }

    return items;
}
