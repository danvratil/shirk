#pragma once

#include "common.h"

#include <QDateTime>
#include <QUrlQuery>
#include <QUrl>

class QJsonValue;

namespace Shirk::SlackAPI
{

class UserInfoRequest : public UserAuthenticatedRequest
{
public:
    static constexpr Method method = Method::GET;
    static constexpr QStringView endpoint = u"users.info";

    QString id;

    QUrlQuery serialize() const;
};

struct UserInfo
{
    QString id;
    QString team_id;
    QString name;
    QString color;
    QString real_name;
    QString tz;
    QString tz_label;
    int tz_offset = 0;
    bool deleted = false;
    struct {
        QString avatar_hash;
        QString status_text;
        QString status_emoji;
        QString real_name;
        QString display_name;
        QString real_name_normalized;
        QString display_name_normalized;
        QString email;
        QUrl image_original;
        QUrl image_24;
        QUrl image_32;
        QUrl image_48;
        QUrl image_72;
        QUrl image_192;
        QUrl image_512;
        QString team;
    } profile;
    bool is_admin = false;
    bool is_owner = false;
    bool is_primary_owner = false;
    bool is_restricted = false;
    bool is_ultra_restricted = false;
    bool is_bot = false;
    QDateTime updated;
    bool is_app_user = false;
    bool has_2fa = false;

    static UserInfo parse(const QJsonValue &value);
};

class UserInfoResponse : public Response
{
public:
    UserInfo info;

    static UserInfoResponse parse(const QJsonValue &val);
};


} // namespace
