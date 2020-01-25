#include "rtm.h"
#include "common_p.h"

#include <QUrlQuery>
#include <QJsonValue>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

QUrlQuery RTMConnectRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(u"batch_presence_aware"_qs, u"1"_qs);
    query.addQueryItem(u"presense_sub"_qs, u"1"_qs);
    return query;
}

RTMConnectResponse RTMConnectResponse::parse(const QJsonValue &value)
{
    RTMConnectResponse resp;
    API_EXTRACT_URL(resp, value, url);
    return resp;
}
