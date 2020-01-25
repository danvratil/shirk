#pragma ocne

#include "common.h"

#include <QStringView>
#include <QUrl>

class QUrlQuery;
class QJsonValue;

namespace Shirk::SlackAPI
{

struct RTMConnectRequest : public AuthenticatedRequest
{
    static constexpr QStringView endpoint = u"rtm.connect";
    static constexpr Method method = Method::GET;

    QUrlQuery serialize() const;
};

struct RTMConnectResponse : public Response
{
    QUrl url;

    static RTMConnectResponse parse(const QJsonValue &value);
};


} // namespace
