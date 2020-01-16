#include "addteampage.h"
#include "gui_debug.h"
#include "core/authcontroller.h"
#include "core/config.h"
#include "core/teamsmodel.h"
#include "core/teamcontroller.h"

using namespace Gui;

AddTeamPage::AddTeamPage(Core::TeamsModel &teams, Core::Environment &environment)
    : QWidget()
    , mEnv(environment)
    , mTeams(teams)
{
    ui.setupUi(this);

    connect(ui.signInButton, &QPushButton::clicked,
            this, [this]() {
                mAuth = std::make_unique<Core::AuthController>(mEnv);
                connect(mAuth.get(), &Core::AuthController::stateChanged,
                        this, &AddTeamPage::authStateChanged);
                mAuth->start();
            });
}

void AddTeamPage::authStateChanged(Core::AuthController::State state)
{
    qCDebug(LOG_GUI) << "Auth state changed:" << state;
    switch (state) {
    case Core::AuthController::State::Error:
        setError(mAuth->error());
        break;
    case Core::AuthController::State::Done: {
        auto controller = std::make_unique<Core::TeamController>(mEnv, mAuth->team());
        auto &team = controller->team();
        mTeams.addTeam(std::move(controller));
        Q_EMIT teamAdded(team);
        break;
    }
    case Core::AuthController::State::None:
    case Core::AuthController::State::RetrievingTeamInfo:
    case Core::AuthController::State::RetrievingToken:
    case Core::AuthController::State::WaitingForBrowser:
        break;
    }
}

void AddTeamPage::setError(const QString &error)
{
    qCWarning(LOG_GUI) << "Auth error:" << error;
}
