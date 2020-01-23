#include "user.h"
#include "slackapi/user.h"
#include "team.h"
#include "usermanager.h"

using namespace Shirk::Core;

User::User(const QString &id, UserManager &manager)
    : mId(id)
    , mManager(manager)
{}

User::~User()
{
    mManager.cancelPendingRequest(this);
}

void User::requestData()
{
    mManager.requestData(this, [this](const User::Id &userId, const UserData &data) {
        Q_ASSERT(mId == userId);
        mData = data;
        Q_EMIT userChanged();
        Q_EMIT statusChanged();
    });
}

QString User::teamId() const
{
    return mManager.team().id();
}

UserData UserData::fromAPI(const SlackAPI::UserInfo &info)
{
    return UserData {
        .name = info.name,
        .realName = info.real_name,
        .displayName = info.profile.display_name,
        .statusText = info.profile.status_text,
        .statusEmoji = info.profile.status_emoji,
        .email = info.profile.email,
        .timeZone = info.tz,
        .timeZoneLabel = info.tz_label,
        .color = info.color,
        .isDeleted = info.deleted,
        .isAdmin = info.is_admin,
        .isOwner = info.is_owner,
        .isBot = info.is_bot,
        .updated = info.updated
    };
}
