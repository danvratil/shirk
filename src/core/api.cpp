#include "api.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QUrlQuery>

using namespace Core::API;

const QString OAuthAccessRequest::endpoint = QStringLiteral("oauth.access");
const QString TeamInfoRequest::endpoint = QStringLiteral("team.info");




QUrlQuery OAuthAccessRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("client_id"), clientId);
    query.addQueryItem(QStringLiteral("client_secret"), clientSecret);
    query.addQueryItem(QStringLiteral("code"), code);
    query.addQueryItem(QStringLiteral("redirect_uri"), redirectUri.toString());
    return query;
}

OAuthAccessResponse OAuthAccessResponse::parse(const QJsonValue &data)
{
    return {{},
            data[QStringLiteral("access_token")].toString(),
            data[QStringLiteral("scope")].toString(),
            data[QStringLiteral("team_name")].toString(),
            data[QStringLiteral("team_id")].toString()};
}

QUrlQuery TeamInfoRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("team"), team);
    return query;
}

TeamInfoResponse TeamInfoResponse::parse(const QJsonValue &data)
{
    TeamInfoResponse resp;
    const auto team = data[QStringLiteral("team")].toObject();
    resp.id = team[QStringLiteral("id")].toString();
    resp.name = team[QStringLiteral("name")].toString();
    resp.domain = team[QStringLiteral("domain")].toString();
    resp.email_domain = team[QStringLiteral("email_domain")].toString();
    const auto icon = data[QStringLiteral("icon")].toObject();
    resp.icon.image_34 = icon[QStringLiteral("image_34")].toString();
    resp.icon.image_44 = icon[QStringLiteral("image_44")].toString();
    resp.icon.image_68 = icon[QStringLiteral("image_68")].toString();
    resp.icon.image_88 = icon[QStringLiteral("image_88")].toString();
    resp.icon.image_102 = icon[QStringLiteral("image_102")].toString();
    resp.icon.image_132 = icon[QStringLiteral("image_132")].toString();
    resp.icon.image_default = icon[QStringLiteral("image_default")].toBool(false);
    resp.enterprise_id = team[QStringLiteral("enterprise_id")].toString();
    resp.enterprise_name = team[QStringLiteral("enterprise_name")].toString();
    return resp;
}
