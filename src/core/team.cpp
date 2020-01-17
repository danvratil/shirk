#include "team.h"
#include "iconloader.h"
#include "config.h"
#include "api/teaminfo.h"

#include <QSettings>
#include <QJsonObject>
#include <QJsonValue>

using namespace Core;

Team::Team(const QString &id, const QString &name, const QString &accessToken)
    : mId(id)
    , mName(name)
    , mAccessToken(accessToken)
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
    return team;
}

void Team::updateFromTeamInfo(const API::TeamInfoResponse &info)
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
}

