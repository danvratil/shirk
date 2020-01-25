#include "files.h"
#include "common_p.h"
#include "utils/stringliterals.h"

#include <QJsonValue>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

File File::parse(const QJsonValue &value)
{
    File file;
    API_EXTRACT_STR(file, value, id);
    API_EXTRACT_INT(file, value, created);
    API_EXTRACT_INT(file, value, updated);
    API_EXTRACT_STR(file, value, name);
    API_EXTRACT_STR(file, value, title);
    API_EXTRACT_STR(file, value, mimetype);
    API_EXTRACT_STR(file, value, filetype);
    API_EXTRACT_STR(file, value, pretty_type);
    API_EXTRACT_STR(file, value, user);
    API_EXTRACT_BOOL(file, value, editable);
    API_EXTRACT_INT(file, value, size);
    API_EXTRACT_STR(file, value, mode);
    API_EXTRACT_BOOL(file, value, is_external);
    API_EXTRACT_STR(file, value, external_type);
    API_EXTRACT_BOOL(file, value, is_public);
    API_EXTRACT_BOOL(file, value, public_url_shared);
    API_EXTRACT_BOOL(file, value, display_as_bot);
    API_EXTRACT_STR(file, value, username);
    API_EXTRACT_STR(file, value, preview);
    API_EXTRACT_STR(file, value, preview_highlight);
    API_EXTRACT_INT(file, value, lines);
    API_EXTRACT_INT(file, value, lines_more);
    API_EXTRACT_URL(file, value, url_private);
    API_EXTRACT_URL(file, value, url_private_download);
    API_EXTRACT_URL(file, value, thumb_64);
    API_EXTRACT_URL(file, value, thumb_88);
    API_EXTRACT_URL(file, value, thumb_360);
    API_EXTRACT_URL(file, value, thumb_360_w);
    API_EXTRACT_URL(file, value, thumb_360_h);
    API_EXTRACT_URL(file, value, thumb_160);
    API_EXTRACT_URL(file, value, thumb_360_gif);
    API_EXTRACT_URL(file, value, thumb_480);
    API_EXTRACT_URL(file, value, thumb_720);
    API_EXTRACT_URL(file, value, thumb_960);
    API_EXTRACT_URL(file, value, thumb_1024);
    API_EXTRACT_INT(file, value, image_exif_rotation);
    API_EXTRACT_INT(file, value, original_w);
    API_EXTRACT_INT(file, value, original_h);
    API_EXTRACT_URL(file, value, deanimate_gif);
    API_EXTRACT_URL(file, value, pjpeg);
    API_EXTRACT_URL(file, value, permalink);
    API_EXTRACT_URL(file, value, permalink_public);
    API_EXTRACT_STR(file, value, initial_comment);
    API_EXTRACT_INT(file, value, comments_count);
    API_EXTRACT_INT(file, value, num_stars);
    API_EXTRACT_BOOL(file, value, is_starred);
    API_EXTRACT_STRLIST(file, value, pinned_to);
    API_EXTRACT_STRLIST(file, value, channels);
    API_EXTRACT_STRLIST(file, value, groups);
    API_EXTRACT_STRLIST(file, value, ims);
    API_EXTRACT_BOOL(file, value, has_rich_preview);

    return file;
}
