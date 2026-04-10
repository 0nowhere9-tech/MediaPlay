#pragma once
#include <QAbstractListModel>
#include <QList>
#include "VideoItem.h"

// Qt list model that exposes a QList<VideoItem> to QML.
// Roles map directly to VideoItem fields.
class VideoModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        SourceRole,
        PageUrlRole,
        VideoUrlRole,
        ThumbnailUrlRole,
        DescriptionRole,
        AuthorRole,
        TagsRole,
        DateRole,
        DurationRole,
        LikesRole,
        DislikesRole,
        ViewsRole,
        RatioRole,
    };
    Q_ENUM(Roles)

    explicit VideoModel(QObject *parent = nullptr);

    // --- QAbstractListModel interface ---
    int           rowCount(const QModelIndex &parent = {}) const override;
    QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;

    // Load items from a JSON file; emits layoutChanged.
    Q_INVOKABLE void loadFromFile(const QString &path);

    // Replace the current item list programmatically.
    void setItems(const QList<VideoItem> &items);

    // Append a single item (used by ExtractionManager for live updates).
    void appendItem(const VideoItem &item);

signals:
    void countChanged();
    void loadError(const QString &message);

private:
    QList<VideoItem> m_items;
};
