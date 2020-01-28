#pragma once

#include "common.h"

#include <QString>
#include <QUrl>

class QUrlQuery;
class QJsonValue;

namespace Shirk::SlackAPI {

class OAuthAccessRequest : public UnauthenticatedRequest
{
public:
    static constexpr const Method method = Method::GET;
    static constexpr const QStringView endpoint = u"oauth.access";

    QString clientId;
    QString clientSecret;
    QString code;
    QUrl redirectUri;

    QUrlQuery serialize() const;
};

class OAuthAccessResponse : public Response
{
public:
    QString access_token;
    QString bot_access_token;
    QString scope;
    QString team_name;
    QString team_id;

    static OAuthAccessResponse parse(const QJsonValue &data);
};

} // namespace
