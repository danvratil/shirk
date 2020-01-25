#include "conversation.h"
#include "slackapi/conversations.h"

using namespace Shirk::Core;

Conversation::Conversation(const QString &id, UserManager &userManager)
    : mId(id)
    , mUserManager(userManager)
{}

void Conversation::updateFromConversation(const SlackAPI::Conversation &conv)
{
    mName = conv.name;
    mPreviousNames = conv.previous_names;
    mCreator = std::make_unique<User>(conv.creator, mUserManager);
    mMemberCount = conv.num_members;
    mUnreadCount = conv.unread_count;
    mUnreadCountDisplay = conv.unread_count_display;
    mLastRead = QDateTime::fromSecsSinceEpoch(conv.last_read);
    mCreated = QDateTime::fromSecsSinceEpoch(conv.created);
    mLatestMessage = QDateTime::fromSecsSinceEpoch(conv.latest);
    mTopic = ConversationInfo{.value = conv.topic.value,
                              .creator = std::make_unique<User>(conv.topic.creator, mUserManager),
                              .lastSet = QDateTime::fromSecsSinceEpoch(conv.topic.last_set)};
    mPurpose = ConversationInfo{.value = conv.purpose.value,
                                .creator = std::make_unique<User>(conv.purpose.creator, mUserManager),
                                .lastSet = QDateTime::fromSecsSinceEpoch(conv.purpose.last_set)};
    mIsMember = conv.is_member;
    mIsArchive = conv.is_archived;
    mIsReadOnly = conv.is_read_only;
    mIsGeneral = conv.is_general;
    mIsChannel = conv.is_channel;
    mIsGroup = conv.is_group;
    mIsIM = conv.is_im;
    mIsMPIM = conv.is_mpim;
    mIsPrivate = conv.is_private;
    mIsShared = conv.is_shared;

    Q_EMIT conversationChanged();
    Q_EMIT lastReadChanged();
    Q_EMIT unreadCountChanged();
    Q_EMIT latestMessageChanged();
}
