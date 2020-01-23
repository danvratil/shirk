#pragma once

#include <QObject>
#include <QDateTime>

#include "user.h"

namespace Shirk::SlackAPI {
class Conversation;
}

namespace Shirk::Core
{

class Conversation : public QObject
{
    Q_OBJECT
public:
    struct Info {
        Q_GADGET
        Q_PROPERTY(QString value MEMBER value CONSTANT)
        Q_PROPERTY(User creator MEMBER creator CONSTANT)
        Q_PROPERTY(QDateTime lastSet MEMBER lastSet CONSTANT)

        const QString value;
        const User creator;
        const QDateTime lastSet;
    };

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY conversationChanged)
    Q_PROPERTY(QStringList previousNames READ previousNames NOTIFY conversationChanged)
    Q_PROPERTY(Shirk::Core::User* creator READ creatorPtr CONSTANT)
    Q_PROPERTY(int memberCount READ memberCount NOTIFY conversationChanged)

    Q_PROPERTY(QDateTime lastRead READ lastRead NOTIFY lastReadChanged)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int unreadCountDisplay READ unreadCountDisplay NOTIFY unreadCountChanged)
    Q_PROPERTY(QDateTime latestMessage READ latestMessage NOTIFY latestMessageChanged)

    Q_PROPERTY(bool isMember READ isMember NOTIFY conversationChanged)
    Q_PROPERTY(bool isArchive READ isArchive NOTIFY conversationChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY conversationChanged)

    Q_PROPERTY(bool isGeneral READ isGeneral NOTIFY conversationChanged)
    Q_PROPERTY(bool isChannel READ isChannel NOTIFY conversationChanged)
    Q_PROPERTY(bool isGroup READ isGroup NOTIFY conversationChanged)
    Q_PROPERTY(bool isIM READ isIM NOTIFY conversationChanged)
    Q_PROPERTY(bool isMPIM READ isMPIM NOTIFY conversationChanged)
    Q_PROPERTY(bool isPrivate READ isPrivate NOTIFY conversationChanged)

    Q_PROPERTY(bool isShared READ isShared NOTIFY conversationChanged)

    Q_PROPERTY(Shirk::Core::Conversation::Info* READ topicPtr NOTIFY conversationChanged)
    Q_PROPERTY(Shirk::Core::Conversation::Info* READ purposePtr NOTIFY conversationChanged)

public:
    Conversation(const QString &id);
    ~Conversation() = default;

    QString id() const { return mId; }
    QString name() const { return mName; }
    QStringList previousNames() const { return mPreviousNames; }
    const User &creator() const { return mCreator; }
    // For Q_PROPERTY...
    User *creatorPtr() const { return &mCreator; }

    int memberCount() const { return mMemberCount; }
    int unreadCount() const { return mUnreadCount; }
    int unreadCountDisplay() const { return mUnreadCountDisplay; }

    QDateTime lastRead() const { return mLastRead; }
    QDateTime created() const { return mCreator; }
    QDateTime latestMessage() const { return mLatestMessage; }

    bool isMember() const { return mIsMember; }
    bool isArchive() const { return mIsArchive; }
    bool isReadOnly() const { return mIsReadOnly; }
    bool isGeneral() const { return mIsGeneral; }
    bool isChannel() const { return mIsChannel; }
    bool isGroup() const { return mIsGroup; }
    bool isIM() const { return mIsIM; }
    bool isMPIM() const { return mIsMPIM; }
    bool isPrivate() const { return mIsPrivate; }
    bool isShared() const { return mIsShared ; }

    const Info &topic() const { return mTopic; }
    const Info &purpose() const { return mPurpose; }
    Info *topicPtr() const { return &mTopic; }
    Info *purposePtr() const { return &mPurpose; }


    void updateFromConversation(const SlackAPI::Conversation &conv);

Q_SIGNALS:
    void conversationChanged();
    void lastReadChanged();
    void unreadCountChanged();
    void latestMessageChanged();

protected:
    Conversation() = default;

private:
    QString mId;
    QString mName;
    QStringList mPreviousNames;
    User mCreator;
    int mMemberCount = 0;
    int mUnreadCount = 0;
    int mUnreadCountDisplay = 0;
    QDateTime mLastRead;
    QDateTime mCreated;
    QDateTime mLatestMessage;
    Info mTopic;
    Info mPurpose;
    bool mIsMember = false;
    bool mIsArchive = false;
    bool mIsReadOnly = false;
    bool mIsGeneral = false;
    bool mIsChannel = false;
    bool mIsGroup = false;
    bool mIsIM = false;
    bool mIsMPIM = false;
    bool mIsPrivate = false;
    bool mIsShared = false;
};
