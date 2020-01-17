#pragma once

#include <QWidget>

#include "core/teamsmodel.h"
#include "ui_addteampage.h"
#include "core/authcontroller.h"

#include <memory>

namespace Shirk::Core {
class Team;
class Environment;
}

namespace Shirk::Gui
{

class AddTeamPage : public QWidget
{
    Q_OBJECT
public:
    explicit AddTeamPage(Core::TeamsModel &teams, Core::Environment &environment);

Q_SIGNALS:
    void teamAdded(Core::Team &);

private Q_SLOTS:
    void authStateChanged(Core::AuthController::State state);

private:
    void setError(const QString &error);

    Ui::AddTeamPage ui;
    Core::Environment &mEnv;
    Core::TeamsModel &mTeams;
    std::unique_ptr<Core::AuthController> mAuth;
};

}
