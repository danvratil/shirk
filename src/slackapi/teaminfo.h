#pragma once

#include "common.h"

#include <QString>
#include <QUrl>

class QJsonValue;
class QUrlQuery;

namespace Shirk::SlackAPI {

class TeamInfoRequest : public AuthenticatedRequest
{
public:
    static constexpr const Method method = Method::GET;
    static constexpr const QStringView endpoint = u"team.info";

    QString team;

    QUrlQuery serialize() const;
};

class TeamInfoResponse : public Response
{
public:
    QString id;
    QString name;
    QString domain;
    QString email_domain;
    struct {
        QUrl image_34;
        QUrl image_44;
        QUrl image_68;
        QUrl image_88;
        QUrl image_102;
        QUrl image_132;
        bool image_default = true;
    } icon;
    QString enterprise_id;
    QString enterprise_name;

    static TeamInfoResponse parse(const QJsonValue &data);
};

} // namespace
