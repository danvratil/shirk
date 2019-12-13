#pragma once

#include <QWidget>

#include "ui_addteampage.h"
#include "core/authcontroller.h"

#include <memory>

namespace Core {
class Config;
class TeamController;
class TeamsModel;
}

namespace Gui
{

class AddTeamPage : public QWidget
{
    Q_OBJECT
public:
    explicit AddTeamPage(Core::Config &config, Core::TeamsModel &teams);

Q_SIGNALS:
    void teamAdded(Core::TeamController *);

private Q_SLOTS:
    void authStateChanged(Core::AuthController::State state);

private:
    Ui::AddTeamPage ui;
    Core::Config &mConfig;
    Core::TeamsModel &mTeams;
    std::unique_ptr<Core::AuthController> mAuth;
};

}
