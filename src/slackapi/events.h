#pragma once

#include <QString>
#include <QStringList>

#include <memory>

#include "files.h"

class QJsonValue;

namespace Shirk::SlackAPI::RTM
{

struct EventException : public std::runtime_error
{
    explicit EventException(const QString &what)
        : std::runtime_error(what.toStdString()) {}
};

#define SHIRK_EVENT_COMMON(name) \
    public: \
        static constexpr QStringView eventName = u##name; \
        static std::unique_ptr<Event> parse(const QJsonValue &);

#define SHIRK_EVENT(klass, type, name) \
    protected: \
        explicit klass() : Event(Type::type) {} \
    SHIRK_EVENT_COMMON(name)

#define SHIRK_MESSAGE_EVENT(klass, type, name) \
    protected: \
        explicit klass(): MessageEvent(SubType::type) {} \
        friend struct MessageEvent; \
    SHIRK_EVENT_COMMON(name) \

struct Event;
std::unique_ptr<Event> parseEvent(const QJsonValue &event);

struct Event
{
    enum Type {
        Unknown,
        AccountsChanged,
        BotAdded,
        BotChanged,
        ChannelArchive,
        ChannelCreated,
        ChannelDeleted,
        ChannelHistoryChanged,
        ChannelJoined,
        ChannelLeft,
        ChannelMarked,
        ChannelRename,
        ChannelUnarchive,
        CommandsChanged,
        DnDUpdated,
        DnDUpdatedUser,
        EmailDomainChanged,
        EmojiChanged,
        ExternalOrgMigrationFinished,
        ExternalOrgMigrationStarted,
        FileChange,
        FileCommentAdded,
        FilecommentDeleted,
        FileCommentEdited,
        FileCreated,
        FileDeleted,
        FilePublic,
        FileShared,
        Goodbye,
        GroupArchive,
        GroupClose,
        GroupDeleted,
        GroupHistoryChanged,
        GroupJoined,
        GroupLeft,
        GroupMarked,
        GroupOpen,
        GroupUnarchive,
        Hello,
        IMClose,
        IMCreated,
        IMHistoryChanged,
        IMMarked,
        IMOpen,
        ManualPresenceChange,
        MemberJoinedChannel,
        Message,
        PINAdded,
        PINRemoved,
        PrefChange,
        PresenceChange,
        PresenceQuery,
        PresenceSub,
        ReactionAdded,
        ReactionRemoved,
        ReconnectUrl,
        StarAdded,
        StarRemoved,
        SubteamCreator,
        SubteamMembersChanged,
        SubteamSelfAdded,
        SubteamSelfRemoved,
        SubteamUpdated,
        TeamDomainChange,
        TeamJoin,
        TeamMigrationStarted,
        TeamPlanChange,
        TeamPrefChange,
        TeamProfileChange,
        TeamProfileDelete,
        TeamProfileReorder,
        TeamRename,
        UserChange,
        UserTyping
    };

    Event() = delete;
    Event(const Event &) = delete;
    Event(Event &&) = delete;
    Event &operator=(const Event &) = delete;
    Event &operator=(Event &&) = delete;

    Type type = Type::Unknown;

protected:
    explicit Event(Type type)
        : type(type)
    {}

};

struct MessageEvent : public Event
{
    SHIRK_EVENT(MessageEvent, Message, "message")

    enum class SubType {
        Unknown,
        Message,
        BotMessage,
        ChannelArchive,
        ChannelJoin,
        ChannelLeave,
        ChannelName,
        ChannelPurpose,
        ChannelTopic,
        ChannelUnarchive,
        EKMAccessDenied,
        FileComment,
        FileMention,
        GroupArchive,
        GroupJoin,
        GroupLeave,
        GroupName,
        GroupPurpose,
        GroupTopic,
        GroupUnarchive,
        MeMessage,
        MessageChanged,
        MessageDeleted,
        MessageReplied,
        PinnedItem,
        ThreadBroadcast,
        UnpinnedItem
    };

    QString channel;
    QString user;
    QString text;
    double ts = .0f;
    bool hidden = false;
    SubType subType = SubType::Message;

protected:
    template<typename EventType>
    static std::unique_ptr<EventType> makeEvent(const QJsonValue &value)
    {
        std::unique_ptr<EventType> event{new EventType{}};
        parseBase(event.get(), value);
        return event;
    }

    static void parseBase(MessageEvent *event, const QJsonValue &);

    explicit MessageEvent(SubType subtype)
        : Event(Event::Type::Message)
        , subType(subtype)
    {}
};
struct BotMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(BotMessageEvent, BotMessage, "bot_message")

    QString bot_id;
    QString username;
    QStringList icons;
};

struct ChannelArchiveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelArchiveMessageEvent, ChannelArchive, "channel_archive")
};

struct ChannelJoinMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelJoinMessageEvent, ChannelJoin, "channel_join")

    QString inviter;
};

struct ChannelLeaveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelLeaveMessageEvent, ChannelLeave, "channel_leave")
};

struct ChannelNameMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelNameMessageEvent, ChannelName, "channel_name")

    QString old_name;
    QString name;
};

struct ChannelPurposeMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelPurposeMessageEvent, ChannelPurpose, "channel_purpose")

    QString purpose;
};

struct ChannelTopicMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelTopicMessageEvent, ChannelTopic, "channel_topic")

    QString topic;
};

struct ChannelUnarchiveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(ChannelUnarchiveMessageEvent, ChannelUnarchive, "channel_unarchive");
};

struct EKMAccessDeniedMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(EKMAccessDeniedMessageEvent, EKMAccessDenied, "ekm_access_denied")
};

struct FileCommentMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(FileCommentMessageEvent, FileComment, "file_comment")

    File file;
    //Comment comment;
};

struct FileMentionMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(FileMentionMessageEvent, FileMention, "file_mention")

    File file;
};

struct GroupArchiveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupArchiveMessageEvent, GroupArchive, "group_archive")

    QStringList members;
};

struct GroupJoinMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupJoinMessageEvent, GroupJoin, "group_join")
};

struct GroupLeaveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupLeaveMessageEvent, GroupLeave, "group_leave")
};

struct GroupNameMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupNameMessageEvent, GroupName, "group_name")

    QString old_name;
    QString name;
};

struct GroupPurposeMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupPurposeMessageEvent, GroupPurpose, "group_purpose")

    QString purpose;
};

struct GroupTopicMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupTopicMessageEvent, GroupTopic, "group_topic")

    QString topic;
};

struct GroupUnarchiveMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(GroupUnarchiveMessageEvent, GroupUnarchive, "group_unarchive")
};

struct MeMessageEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(MeMessageEvent, MeMessage, "me_message")
};

struct MessageChangedEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(MessageChangedEvent, MessageChanged, "message_changed")

    QString edited_user;
    double edited_ts = .0f;
};

struct MessageDeletedEvent : public MessageEvent
{
    SHIRK_MESSAGE_EVENT(MessageDeletedEvent, MessageDeleted, "message_deleted")

    double deleted_ts = .0f;
};

struct MemberJoinedChannelEvent: public Event
{
    SHIRK_EVENT(MemberJoinedChannelEvent, MemberJoinedChannel, "member_joined_channel")

    QString user;
    QString team;
    QString channel;
    QString channel_type;
    QString inviter;
};

} // namespace
