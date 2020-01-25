#pragma once

#include "common.h"

#include <QString>
#include <QStringList>
#include <QUrl>

class QJsonValue;

namespace Shirk::SlackAPI
{

struct File
{
    QString id;
    uint32_t created = 0;
    uint32_t updated = 0;
    QString name;
    QString title;
    QString mimetype;
    QString filetype;
    QString pretty_type;
    QString user;
    bool editable = false;
    uint64_t size = 0;
    QString mode;
    bool is_external = false;
    QString external_type;
    bool is_public = false;
    bool public_url_shared = false;
    bool display_as_bot = false;
    QString username;
    QString preview;
    QString preview_highlight;
    std::optional<int> lines;
    std::optional<int> lines_more;
    QUrl url_private;
    QUrl url_private_download;
    QUrl thumb_64;
    QUrl thumb_88;
    QUrl thumb_360;
    QUrl thumb_360_w;
    QUrl thumb_360_h;
    QUrl thumb_160;
    QUrl thumb_360_gif;
    QUrl thumb_480;
    QUrl thumb_720;
    QUrl thumb_960;
    QUrl thumb_1024;
    std::optional<int> image_exif_rotation;
    std::optional<int> original_w;
    std::optional<int> original_h;
    QUrl deanimate_gif;
    QUrl pjpeg;
    QUrl permalink;
    QUrl permalink_public;
    QString initial_comment;
    int comments_count = 0;
    int num_stars = 0;
    bool is_starred = false;
    QStringList pinned_to;
    QStringList channels;
    QStringList groups;
    QStringList ims;
    bool has_rich_preview = false;

    // TODO: Reactions

    static File parse(const QJsonValue &);
};



} // namespace
