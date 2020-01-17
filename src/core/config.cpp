#include "config.h"
#include <QSettings>

using namespace Shirk::Core;

Config::Config() = default;

QStringList Config::teamIds() const
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("teams"));
    return settings.childGroups();
}

std::unique_ptr<QSettings> Config::settingsForTeam(const QString &teamId)
{
    auto settings = std::make_unique<QSettings>();
    settings->beginGroup(QStringLiteral("teams/%1").arg(teamId));
    return settings;
}
