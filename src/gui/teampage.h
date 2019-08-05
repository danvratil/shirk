#pragma once

#include <QWidget>

namespace Core
{
class TeamController;
class Config;
}

namespace Gui
{

class TeamPage : public QWidget
{
    Q_OBJECT
public:
    TeamPage(Core::TeamController &team, Core::Config &config);

    Core::TeamController &team() const { return mTeam; }


private:
    Core::TeamController &mTeam;
    Core::Config &mConfig;
};

}
