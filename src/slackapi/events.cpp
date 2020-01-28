#include "events.h"
#include "common_p.h"
#include "utils/stringliterals.h"

#include <QJsonValue>
#include <QJsonObject>
#include <QStringView>
#include <QMetaObject>
#include <QMetaEnum>
#include <QDebug>

#include <type_traits>

using namespace Shirk::SlackAPI::RTM;
using namespace Shirk::StringLiterals;

namespace {

struct Entry {
    using ParseFunc = std::unique_ptr<Event>(*)(const QJsonValue &);

    constexpr Entry(QStringView name, ParseFunc func)
        : name(name), parseFunc(func)
    {}

    const QStringView name;
    const ParseFunc parseFunc;
};

template<typename ... T>
constexpr std::array<Entry, sizeof...(T)> makeEventRegistry()
{
    return {Entry{T::eventName, T::parse} ...};
}

constexpr const auto eventRegistry = makeEventRegistry<
        MessageEvent,
        MemberJoinedChannelEvent
    >();

constexpr const auto messageEventRegistry = makeEventRegistry<
        BotMessageEvent,
        ChannelArchiveMessageEvent,
        ChannelJoinMessageEvent,
        ChannelLeaveMessageEvent,
        ChannelNameMessageEvent,
        ChannelPurposeMessageEvent,
        ChannelTopicMessageEvent,
        ChannelUnarchiveMessageEvent,
        EKMAccessDeniedMessageEvent,
        FileCommentMessageEvent,
        FileMentionMessageEvent,
        GroupArchiveMessageEvent,
        GroupJoinMessageEvent,
        GroupLeaveMessageEvent,
        GroupNameMessageEvent,
        GroupPurposeMessageEvent,
        GroupTopicMessageEvent,
        GroupUnarchiveMessageEvent,
        MeMessageEvent,
        MessageChangedEvent,
        MessageDeletedEvent
    >();

template<typename Registry>
auto findInRegistry(const Registry &registry, QStringView type)
{
    return std::find_if(registry.cbegin(), registry.cend(), [type](const auto &v) { return v.name == type; });
}

} // namespace

QDebug operator<<(QDebug debug, EventType type)
{
    constexpr auto &mo = Shirk::SlackAPI::RTM::staticMetaObject;
    const auto idx = mo.indexOfEnumerator("Type");
    Q_ASSERT(idx > -1);
    const auto enumerator = mo.enumerator(idx);
    return debug.noquote() << enumerator.valueToKey(static_cast<int>(type));
}


std::unique_ptr<Event> Shirk::SlackAPI::RTM::parseEvent(const QJsonValue &value)
{
    const auto type = value["type"_ql1].toString();
    const auto parser = findInRegistry(eventRegistry, type);
    if (parser == eventRegistry.cend()) {
        throw EventException(u"Unsupported event type '%1'"_qs.arg(type));
    }

    return parser->parseFunc(value);
}

std::unique_ptr<Event> MessageEvent::parse(const QJsonValue &value)
{
    const auto obj = value.toObject();
    if (obj.contains("subtype"_ql1)) {
        const auto subtype = obj["subtype"_ql1].toString();
        const auto parser = findInRegistry(messageEventRegistry, subtype);
        if (parser == messageEventRegistry.cend()) {
            throw EventException(u"Unsupported message event subtype '%1'"_qs.arg(subtype));
        }
        return parser->parseFunc(value);
    }

    return makeEvent<MessageEvent>(value);
}

void MessageEvent::parseBase(MessageEvent *event, const QJsonValue &value)
{
    event->channel = value["channel"_ql1].toString();
    event->user = value["user"_ql1].toString();
    event->text = value["text"_ql1].toString();
    event->ts = value["ts"_ql1].toDouble();
    if (const auto hidden = value["hidden"_ql1]; hidden.type() == QJsonValue::Bool) {
        event->hidden = hidden.toBool();
    }
}

std::unique_ptr<Event> BotMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<BotMessageEvent>(value);
    event->bot_id = value["bot_id"_ql1].toString();
    event->username = value["username"_ql1].toString();
    event->icons = toStringList(value["icons"_ql1]);
    return event;
}

std::unique_ptr<Event> ChannelArchiveMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<ChannelArchiveMessageEvent>(value);
}

std::unique_ptr<Event> ChannelJoinMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<ChannelJoinMessageEvent>(value);
    event->inviter = value["inviter"_ql1].toString();
    return event;
}

std::unique_ptr<Event> ChannelLeaveMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<ChannelLeaveMessageEvent>(value);
}

std::unique_ptr<Event> ChannelNameMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<ChannelNameMessageEvent>(value);
    event->name = value["name"_ql1].toString();
    event->old_name = value["old_name"_ql1].toString();
    return event;
}

std::unique_ptr<Event> ChannelPurposeMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<ChannelPurposeMessageEvent>(value);
    event->purpose = value["purpose"_ql1].toString();
    return event;
}

std::unique_ptr<Event> ChannelTopicMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<ChannelTopicMessageEvent>(value);
    event->topic = value["topic"_ql1].toString();
    return event;
}

std::unique_ptr<Event> ChannelUnarchiveMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<ChannelUnarchiveMessageEvent>(value);
}

std::unique_ptr<Event> EKMAccessDeniedMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<EKMAccessDeniedMessageEvent>(value);
}

std::unique_ptr<Event> FileCommentMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<FileCommentMessageEvent>(value);
    event->file = File::parse(value["file"_ql1]);
    //event->comment = Comment::parse(value["comment"_ql1]);
    return event;
}

std::unique_ptr<Event> FileMentionMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<FileMentionMessageEvent>(value);
    event->file = File::parse(value["file"_ql1]);
    return event;
}

std::unique_ptr<Event> GroupArchiveMessageEvent::parse(const QJsonValue &value)
{
    auto event =  makeEvent<GroupArchiveMessageEvent>(value);
    event->members = toStringList(value["members"_ql1]);
    return event;
}

std::unique_ptr<Event> GroupJoinMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<GroupJoinMessageEvent>(value);
}

std::unique_ptr<Event> GroupLeaveMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<GroupLeaveMessageEvent>(value);
}

std::unique_ptr<Event> GroupNameMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<GroupNameMessageEvent>(value);
    event->name = value["name"_ql1].toString();
    event->old_name = value["old_name"_ql1].toString();
    return event;
}

std::unique_ptr<Event> GroupPurposeMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<GroupPurposeMessageEvent>(value);
    event->purpose = value["purpose"_ql1].toString();
    return event;
}

std::unique_ptr<Event> GroupTopicMessageEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<GroupTopicMessageEvent>(value);
    event->topic = value["topic"_ql1].toString();
    return event;
}

std::unique_ptr<Event> GroupUnarchiveMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<GroupUnarchiveMessageEvent>(value);
}

std::unique_ptr<Event> MeMessageEvent::parse(const QJsonValue &value)
{
    return makeEvent<MeMessageEvent>(value);
}

std::unique_ptr<Event> MessageChangedEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<MessageChangedEvent>(value);
    const auto message = value["message"_ql1];
    event->text = message["text"_ql1].toString();
    event->user = message["user"_ql1].toString();
    event->edited_ts = message["edited"_ql1]["ts"_ql1].toDouble();
    event->edited_user = message["edited"_ql1]["user"_ql1].toString();
    return event;
}

std::unique_ptr<Event> MessageDeletedEvent::parse(const QJsonValue &value)
{
    auto event = makeEvent<MessageDeletedEvent>(value);
    event->deleted_ts = value["deleted_ts"_ql1].toDouble();
    return event;
}

std::unique_ptr<Event> MemberJoinedChannelEvent::parse(const QJsonValue &value)
{
    std::unique_ptr<MemberJoinedChannelEvent> event{new MemberJoinedChannelEvent{}};
    event->user = value["user"_ql1].toString();
    event->channel = value["channel"_ql1].toString();
    event->channel_type = value["channel_type"_ql1].toString();
    event->team = value["team"_ql1].toString();
    event->inviter = value["inviter"_ql1].toString();
    return event;
}
