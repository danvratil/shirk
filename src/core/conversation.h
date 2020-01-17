#pragma once

#include <QObject>

namespace Shirk::SlackAPI {
class Conversation;
}

namespace Shirk::Core
{

class Conversation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY conversationChanged)
    Q_PROPERTY(QStringList previousNames READ previousNames NOTIFY conversationChanged)
    Q_PROPERTY(User creator READ creator CONSTANT)
    Q_PROPERTY(int memberCount READ memberCount NOTIFY conversationChanged)

    Q_PROPERTY(QDateTime lastRead READ lastRead NOTIFY lastReadChanged)
    Q_PROPERTY(QDateTime created READ created CONSTANT)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int unreadCountDisplay READ unreadCountDisplay NOTIFY unreadCountChanged)
    Q_PROPERTY(QDateTime latestMessage READ latestMessage NOTIFY latestMessageChanged)

    Q_PROPERTY(bool isMember READ isMember NOTIFY conversationChanged)
    Q_PROPERTY(bool isArchive READ isArchive NOTIFY conversationChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY conversationChanged)

public:
    Conversation(const QString &id);
    ~Conversation() = default;

    QString id() const { return mId; }
    QString name() const { return mName; }

    void updateFromConversation(const SlackAPI::Conversation &conv);

protected:
    Conversation() = default;

private:
    QString mId;
    QString mName;
};
