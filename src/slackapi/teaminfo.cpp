#include "teaminfo.h"
#include "common_p.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QUrlQuery>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

QUrlQuery TeamInfoRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(u"team"_qs, team);
    return query;
}

TeamInfoResponse TeamInfoResponse::parse(const QJsonValue &data)
{
    TeamInfoResponse resp;
    const auto team = data["team"_ql1].toObject();
    API_EXTRACT_STR(resp, team, id);
    API_EXTRACT_STR(resp, team, name);
    API_EXTRACT_STR(resp, team, domain);
    API_EXTRACT_STR(resp, team, email_domain);
    API_EXTRACT_STR(resp, team, enterprise_id);
    API_EXTRACT_STR(resp, team, enterprise_name);

    const auto icon = data["icon"_ql1].toObject();
    API_EXTRACT_STR(resp.icon, icon, image_34);
    API_EXTRACT_STR(resp.icon, icon, image_44);
    API_EXTRACT_STR(resp.icon, icon, image_68);
    API_EXTRACT_STR(resp.icon, icon, image_88);
    API_EXTRACT_STR(resp.icon, icon, image_102);
    API_EXTRACT_BOOL(resp.icon, icon, image_default);

    return resp;
}

