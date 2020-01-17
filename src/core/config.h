#pragma once

#include <QSettings>

#include <memory>

namespace Shirk::Core {

class Config
{
public:
    Config();

    QStringList teamIds() const;
    std::unique_ptr<QSettings> settingsForTeam(const QString &teamId);
};

}
