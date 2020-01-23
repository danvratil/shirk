#include "user.h"
#include "common_p.h"

#include <QUrlQuery>
#include <QJsonValue>
#include <QJsonObject>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

QUrlQuery UserInfoRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(u"user"_qs, id);
    return query;
}

UserInfo UserInfo::parse(const QJsonValue &value)
{
    UserInfo info;
    const auto user = value.toObject();
    API_EXTRACT_STR(info, user, id);
    API_EXTRACT_STR(info, user, team_id);
    API_EXTRACT_STR(info, user, name);
    API_EXTRACT_STR(info, user, color);
    API_EXTRACT_STR(info, user, real_name);
    API_EXTRACT_STR(info, user, tz);
    API_EXTRACT_STR(info, user, tz_label);
    API_EXTRACT_INT(info, user, tz_offset);
    API_EXTRACT_BOOL(info, user, deleted);
    API_EXTRACT_BOOL(info, user, is_admin);
    API_EXTRACT_BOOL(info, user, is_owner);
    API_EXTRACT_BOOL(info, user, is_primary_owner);
    API_EXTRACT_BOOL(info, user, is_restricted);
    API_EXTRACT_BOOL(info, user, is_ultra_restricted);
    API_EXTRACT_BOOL(info, user, is_bot);
    API_EXTRACT_DATETIME(info, user, updated);
    API_EXTRACT_BOOL(info, user, is_app_user);
    API_EXTRACT_BOOL(info, user, has_2fa);
    const auto profile = user["profile"_ql1].toObject();
    API_EXTRACT_STR(info.profile, profile, avatar_hash);
    API_EXTRACT_STR(info.profile, profile, status_text);
    API_EXTRACT_STR(info.profile, profile, status_emoji);
    API_EXTRACT_STR(info.profile, profile, real_name);
    API_EXTRACT_STR(info.profile, profile, display_name);
    API_EXTRACT_STR(info.profile, profile, real_name_normalized);
    API_EXTRACT_STR(info.profile, profile, display_name_normalized);
    API_EXTRACT_STR(info.profile, profile, email);
    API_EXTRACT_URL(info.profile, profile, image_original);
    API_EXTRACT_URL(info.profile, profile, image_24);
    API_EXTRACT_URL(info.profile, profile, image_32);
    API_EXTRACT_URL(info.profile, profile, image_48);
    API_EXTRACT_URL(info.profile, profile, image_72);
    API_EXTRACT_URL(info.profile, profile, image_192);
    API_EXTRACT_URL(info.profile, profile, image_512);
    API_EXTRACT_STR(info.profile, profile, team);

    return info;
}

UserInfoResponse UserInfoResponse::parse(const QJsonValue &value)
{
    UserInfoResponse resp;
    const auto data = value.toObject();
    resp.info = UserInfo::parse(data["user"_ql1]);
    return resp;
}

