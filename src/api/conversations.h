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

class ConversationsListRequest : public AuthenticatedRequest
{
public:
    static constexpr const Method method = Method::GET;
    static constexpr const QStringView endpoint = u"conversations.list";

    enum class Type {
        PublicChannel,
        PrivateChannel,
        MPIM,
        IM
    };

    QString cursor;
    bool exclude_archived = false;
    int limit = 100;
    QVector<Type> types = {Type::PublicChannel};

    QUrlQuery serialize() const;
};

struct Conversation
{
    QString id;
    QString name;
    QString name_normalized;
    QStringList previous_names;
    QString creator;
    QString locale;
    QStringList pending_shared;
    int num_members;

    uint32_t last_read = 0;
    uint32_t created = 0;
    uint32_t unread_count = 0;
    uint32_t unread_count_display = 0;
    uint32_t latest = 0;
    bool is_member = true;

    bool is_archived = false;
    bool is_read_only = false;

    bool is_general = false;
    bool is_channel = false;
    bool is_group = false;
    bool is_im = false;
    bool is_mpim = false;
    bool is_private = false;

    bool is_shared = false;
    bool is_ext_shared = false;
    bool is_org_shared = false;
    bool is_pending_ext_shared = false;
    struct {
        QString value;
        QString creator;
        uint32_t last_set;
    } topic;
    struct {
        QString value;
        QString creator;
        uint32_t last_set;
    } purpose;

    static Conversation parse(const QJsonValue &data);
};


class ConversationsListResponse : public Response
{
public:
    QVector<Conversation> channels;

    static ConversationsListResponse parse(const QJsonValue &data);
};

}
