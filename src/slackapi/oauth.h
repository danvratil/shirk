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
    QString accessToken;
    QString scope;
    QString teamName;
    QString teamId;

    static OAuthAccessResponse parse(const QJsonValue &data);
};

} // namespace
