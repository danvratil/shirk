#pragma once

#include <QWidget>
#include "ui_teampage.h"

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
    Ui::TeamPage ui;
    Core::TeamController &mTeam;
    Core::Environment &mEnv;
};

}
