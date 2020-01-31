#pragma once

#include <QWidget>

namespace Shirk::Core
{
class Environment;
class TeamController;
}

namespace Shirk::Gui
{

class TeamPage : public QWidget
{
    Q_OBJECT
public:
    TeamPage(Core::TeamController &team, Core::Environment &environment);

    Core::TeamController &team() const {
        return mTeam;
    }

private:
    Core::TeamController &mTeam;
    Core::Environment &mEnv;
};

}
