#include "SettingsManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>

SettingsManager::SettingsManager(const QString &filePath, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
{
    load();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

QVariant SettingsManager::get(const QString &key, const QVariant &fallback) const
{
    QVariant v = readNested(m_data, key);
    return v.isValid() ? v : fallback;
}

void SettingsManager::set(const QString &key, const QVariant &value)
{
    writeNested(m_data, key, value);
}

void SettingsManager::save()
{
    QFile f(m_filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "SettingsManager: cannot write to" << m_filePath;
        return;
    }
    f.write(QJsonDocument(m_data).toJson(QJsonDocument::Indented));
    f.close();
    qDebug() << "SettingsManager: saved to" << m_filePath;
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

void SettingsManager::load()
{
    QFile f(m_filePath);
    if (!f.exists()) {
        qDebug() << "SettingsManager: no settings file yet, starting fresh";
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "SettingsManager: cannot read" << m_filePath;
        return;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "SettingsManager: bad JSON in" << m_filePath << "—" << err.errorString();
        return;
    }
    m_data = doc.object();
    qDebug() << "SettingsManager: loaded from" << m_filePath;
}

// ---------------------------------------------------------------------------
// Nested key helpers  ("section/key" → { "section": { "key": value } })
// ---------------------------------------------------------------------------

QVariant SettingsManager::readNested(const QJsonObject &root, const QString &key)
{
    const int slash = key.indexOf('/');
    if (slash == -1) {
        // Leaf
        const QJsonValue v = root.value(key);
        if (v.isUndefined()) return {};
        if (v.isBool())   return v.toBool();
        if (v.isDouble()) return v.toDouble();
        if (v.isString()) return v.toString();
        return {};
    }
    // Recurse into sub-object
    const QString section = key.left(slash);
    const QString rest    = key.mid(slash + 1);
    const QJsonValue sub  = root.value(section);
    if (!sub.isObject()) return {};
    return readNested(sub.toObject(), rest);
}

void SettingsManager::writeNested(QJsonObject &root, const QString &key, const QVariant &value)
{
    const int slash = key.indexOf('/');
    if (slash == -1) {
        // Leaf — write the value
        switch (value.typeId()) {
        case QMetaType::Bool:   root[key] = value.toBool();   break;
        case QMetaType::Int:    root[key] = value.toInt();    break;
        case QMetaType::Double: root[key] = value.toDouble(); break;
        case QMetaType::Float:  root[key] = static_cast<double>(value.toFloat()); break;
        default:                root[key] = value.toString(); break;
        }
        return;
    }
    // Recurse into sub-object, creating it if absent
    const QString section = key.left(slash);
    const QString rest    = key.mid(slash + 1);
    QJsonObject sub = root.value(section).toObject();
    writeNested(sub, rest, value);
    root[section] = sub;
}
