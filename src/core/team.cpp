#include "team.h"
#include "iconloader.h"
#include "config.h"
#include "slackapi/teaminfo.h"

#include <QSettings>
#include <QJsonObject>
#include <QJsonValue>

using namespace Shirk::Core;

Team::Team(const QString &id, const QString &name, const QString &accessToken, const QString &botAccessToken)
    : mId(id)
    , mName(name)
    , mAccessToken(accessToken)
    , mBotAccessToken(botAccessToken)
{}

QString Team::id() const
{
    return mId;
}

QString Team::name() const
{
    return mName;
}

QString Team::domain() const
{
    return mDomain;
}

QString Team::accessToken() const
{
    return mAccessToken;
}

QString Team::botAccessToken() const
{
    return mBotAccessToken;
}

QIcon Team::icon() const
{
    if (mIcon.has_value()) {
        return *mIcon;
    } else {
        IconLoader::load(mIconUrl, [that = const_cast<Team*>(this)](const QIcon &icon) {
            that->mIcon = icon;
            Q_EMIT that->teamChanged();
        });
        if (mIcon.has_value()) {
            return *mIcon;
        } else {
            return QIcon::fromTheme(QStringLiteral("image-loading-symbolic"));
        }
    }
}

std::unique_ptr<Team> Team::fromSettings(QSettings *settings)
{
    std::unique_ptr<Team> team(new Team);
    team->mId = settings->value(QStringLiteral("id")).toString();
    team->mName = settings->value(QStringLiteral("name")).toString();
    team->mDomain = settings->value(QStringLiteral("domain")).toString();
    team->mIconUrl = settings->value(QStringLiteral("icon")).toUrl();
    // TODO: Store optionally in KWallet?
    team->mAccessToken = settings->value(QStringLiteral("accessToken")).toString();
    team->mBotAccessToken = settings->value(QStringLiteral("botAccessToken")).toString();
    return team;
}

void Team::updateFromTeamInfo(const SlackAPI::TeamInfoResponse &info)
{
    mId = info.id;
    mName = info.name;
    mDomain = info.domain;
    mIconUrl = info.icon.image_132;
    Q_EMIT teamChanged();
}

void Team::updateConfig(QSettings *settings)
{
    settings->setValue(QStringLiteral("id"), mId);
    settings->setValue(QStringLiteral("name"), mName);
    settings->setValue(QStringLiteral("domain"), mDomain);
    settings->setValue(QStringLiteral("icon"), mIconUrl);
    settings->setValue(QStringLiteral("accessToken"), mAccessToken);
    settings->setValue(QStringLiteral("botAccessToken"), mBotAccessToken);
}

