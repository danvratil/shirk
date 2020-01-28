#include "oauth.h"
#include "utils/stringliterals.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QUrlQuery>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

QUrlQuery OAuthAccessRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(u"client_id"_qs, clientId);
    query.addQueryItem(u"client_secret"_qs, clientSecret);
    query.addQueryItem(u"code"_qs, code);
    query.addQueryItem(u"redirect_uri"_qs, redirectUri.toString());
    return query;
}

OAuthAccessResponse OAuthAccessResponse::parse(const QJsonValue &data)
{
    return {{},
            data["access_token"_ql1].toString(),
            data["bot"_ql1]["bot_access_token"_ql1].toString(),
            data["scope"_ql1].toString(),
            data["team_name"_ql1].toString(),
            data["team_id"_ql1].toString()};
}

