#include "VideoModel.h"
#include "JsonLoader.h"
#include <QDebug>

VideoModel::VideoModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int VideoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

QVariant VideoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.size())
        return {};

    const VideoItem &v = m_items.at(index.row());

    switch (role) {
    case TitleRole:        return v.title;
    case SourceRole:       return v.source;
    case PageUrlRole:      return v.pageUrl;
    case VideoUrlRole:     return v.videoUrl;
    case ThumbnailUrlRole: return v.thumbnailUrl;
    case DescriptionRole:  return v.description;
    case AuthorRole:       return v.author;
    case TagsRole:         return v.tags;
    case DateRole:         return v.friendlyDate();
    case DurationRole:     return v.durationString();
    case LikesRole:        return v.likes;
    case DislikesRole:     return v.dislikes;
    case ViewsRole:        return v.views;
    case RatioRole:        return v.ratio;
    default:               return {};
    }
}

QHash<int,QByteArray> VideoModel::roleNames() const
{
    return {
        { TitleRole,        "title"        },
        { SourceRole,       "source"       },
        { PageUrlRole,      "pageUrl"      },
        { VideoUrlRole,     "videoUrl"     },
        { ThumbnailUrlRole, "thumbnailUrl" },
        { DescriptionRole,  "description"  },
        { AuthorRole,       "author"       },
        { TagsRole,         "tags"         },
        { DateRole,         "date"         },
        { DurationRole,     "duration"     },
        { LikesRole,        "likes"        },
        { DislikesRole,     "dislikes"     },
        { ViewsRole,        "views"        },
        { RatioRole,        "ratio"        },
    };
}

void VideoModel::loadFromFile(const QString &path)
{
    QString err;
    QList<VideoItem> items = JsonLoader::load(path, err);
    if (!err.isEmpty()) {
        qWarning() << "VideoModel::loadFromFile error:" << err;
        emit loadError(err);
        return;
    }
    setItems(items);
}

void VideoModel::setItems(const QList<VideoItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
    emit countChanged();
}

void VideoModel::appendItem(const VideoItem &item)
{
    const int row = m_items.size();
    beginInsertRows({}, row, row);
    m_items.append(item);
    endInsertRows();
    emit countChanged();
}
