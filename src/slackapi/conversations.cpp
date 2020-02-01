#include "conversations.h"
#include "common_p.h"
#include "utils/stringliterals.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QUrlQuery>

using namespace Shirk::SlackAPI;
using namespace Shirk::StringLiterals;

Conversation Conversation::parse(const QJsonValue &data)
{
    Conversation resp;
    const auto chan = data.toObject();
    API_EXTRACT_STR(resp, chan, id);
    API_EXTRACT_STR(resp, chan, name);
    API_EXTRACT_STR(resp, chan, user);
    API_EXTRACT_STR(resp, chan, name_normalized);
    API_EXTRACT_STRLIST(resp, chan, previous_names);
    API_EXTRACT_STR(resp, chan, creator);
    API_EXTRACT_STR(resp, chan, locale);
    API_EXTRACT_STRLIST(resp, chan, pending_shared);
    API_EXTRACT_INT(resp, chan, num_members);

    API_EXTRACT_INT(resp, chan, last_read);
    API_EXTRACT_INT(resp, chan, created);
    API_EXTRACT_INT(resp, chan, unread_count);
    API_EXTRACT_INT(resp, chan, unread_count_display);
    API_EXTRACT_INT(resp, chan, latest);
    API_EXTRACT_BOOL(resp, chan, is_member);

    API_EXTRACT_BOOL(resp, chan, is_archived);
    API_EXTRACT_BOOL(resp, chan, is_general);
    API_EXTRACT_BOOL(resp, chan, is_group);
    API_EXTRACT_BOOL(resp, chan, is_im);
    API_EXTRACT_BOOL(resp, chan, is_mpim);
    API_EXTRACT_BOOL(resp, chan, is_private);

    API_EXTRACT_BOOL(resp, chan, is_shared);
    API_EXTRACT_BOOL(resp, chan, is_ext_shared);
    API_EXTRACT_BOOL(resp, chan, is_pending_ext_shared);

    const auto topic = data["topic"_ql1].toObject();
    API_EXTRACT_STR(resp.topic, topic, value);
    API_EXTRACT_STR(resp.topic, topic, creator);
    API_EXTRACT_INT(resp.topic, topic, last_set);

    const auto purpose = data["purpose"_ql1].toObject();
    API_EXTRACT_STR(resp.purpose, purpose, value);
    API_EXTRACT_STR(resp.purpose, purpose, creator);
    API_EXTRACT_INT(resp.purpose, purpose, last_set);

    return resp;
}

QUrlQuery ConversationsListRequest::serialize() const
{
    QUrlQuery query;
    query.addQueryItem(u"cursor"_qs, cursor);
    query.addQueryItem(u"exclude_archived"_qs, bool2str(exclude_archived));
    query.addQueryItem(u"limit"_qs, QString::number(limit));
    QStringList type_str;
    for (auto type : types) {
        switch (type) {
        case Type::PublicChannel:
            type_str.push_back(u"public_channel"_qs);
            break;
        case Type::PrivateChannel:
            type_str.push_back(u"private_channel"_qs);
            break;
        case Type::IM:
            type_str.push_back(u"im"_qs);
            break;
        case Type::MPIM:
            type_str.push_back(u"mpim"_qs);
            break;
        }
    }
    query.addQueryItem(u"types"_qs, type_str.join(','));

    return query;
}

ConversationsListResponse ConversationsListResponse::parse(const QJsonValue &value)
{
    ConversationsListResponse resp;
    const auto convs = value["channels"_ql1].toArray();
    for (const auto &conv : convs) {
        resp.channels.push_back(Conversation::parse(conv));
    }

    return resp;
}

