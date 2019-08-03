#pragma once

#include <QString>
#include <QUrl>
#include <QUrlQuery>

class QJsonValue;

namespace Core::API {

class UnauthenticatedRequest {};
class AuthenticatedRequest {};
class Response {};

enum class Method {
    GET,
    POST,
    PUT,
    DELETE
};

class OAuthAccessRequest : public UnauthenticatedRequest
{
public:
    static const Method method = Method::GET;
    static const QString endpoint;

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


class TeamInfoRequest : public AuthenticatedRequest
{
public:
    static const Method method = Method::GET;
    static const QString endpoint;

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

}
