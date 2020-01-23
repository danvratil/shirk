#pragma once

#include "utils/compat.h"

#include <QObject>
#include <QIcon>
#include <QDateTime>
#include <QUrl>

namespace Shirk::SlackAPI
{
class UserInfo;
}

namespace Shirk::Core
{

class UserManager;

struct UserData
{
    QString name;
    QString realName;
    QString displayName;
    QString statusText;
    QString statusEmoji;
    QString email;
    QString timeZone;
    QString timeZoneLabel;
    QIcon avatar;
    QColor color;
    QDateTime updated;
    bool isDeleted = false;
    bool isAdmin = false;
    bool isOwner = false;
    bool isBot = false;

    QIcon avatarCache;

    static UserData fromAPI(const SlackAPI::UserInfo &user);
};

class User : public QObject
{
    Q_OBJECT
public:
    using Id = QString;

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString teamId READ teamId CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY userChanged)
    Q_PROPERTY(QString realName READ realName NOTIFY userChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY userChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)
    Q_PROPERTY(QString statusEmoji READ statusEmoji NOTIFY statusChanged)
    Q_PROPERTY(QString email READ email NOTIFY userChanged)
    Q_PROPERTY(QString timeZone READ timeZone NOTIFY userChanged)
    Q_PROPERTY(QString timeZoneLabel READ timeZoneLabel NOTIFY userChanged)
    Q_PROPERTY(QColor color READ color NOTIFY userChanged)
    Q_PROPERTY(QIcon avatar READ avatar NOTIFY userChanged)
    Q_PROPERTY(bool isDeleted READ isDeleted NOTIFY userChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY userChanged)
    Q_PROPERTY(bool isOwner READ isOwner NOTIFY userChanged)
    Q_PROPERTY(bool isBot READ isBot NOTIFY userChanged)
    Q_PROPERTY(QDateTime updated READ updated NOTIFY userChanged)

public:
    explicit User(const QString &id, UserManager &manager);
    ~User() override;

    User::Id id() const { return mId; }
    QString teamId() const;
    QString name() const { return getData(&UserData::name); }
    QString realName() const { return getData(&UserData::realName); }
    QString displayName() const { return getData(&UserData::displayName); }
    QString statusText() const { return getData(&UserData::displayName); }
    QString statusEmoji() const { return getData(&UserData::statusEmoji); }
    QString email() const { return getData(&UserData::email); }
    QString timeZone() const { return getData(&UserData::timeZone); }
    QString timeZoneLabel() const { return getData(&UserData::timeZoneLabel); }
    QColor color() const { return getData(&UserData::color); }
    QIcon avatar() const { return getData(&UserData::avatar); }

    bool isDeleted() const { return getData(&UserData::isDeleted); }
    bool isAdmin() const { return getData(&UserData::isAdmin); }
    bool isOwner() const { return getData(&UserData::isOwner); }
    bool isBot() const { return getData(&UserData::isBot); }

    QDateTime updated() const { return getData(&UserData::updated); }

Q_SIGNALS:
    void userChanged();
    void statusChanged();

private:
    template<typename T>
    T getData(T UserData::*value) const;
    void requestData();

    User::Id mId;
    std::optional<std::reference_wrapper<const UserData>> mData;
    UserManager &mManager;
};


template<typename T>
T User::getData(T UserData::*value) const
{
    if (mData.has_value()) {
        return mData->get().*value;
    }
    const_cast<User *>(this)->requestData();
    return T{};
}


}



