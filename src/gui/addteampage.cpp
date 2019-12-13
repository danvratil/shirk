#include "addteampage.h"
#include "core/authcontroller.h"
#include "core/config.h"
#include "core/teamsmodel.h"
#include "core/teamcontroller.h""

using namespace Gui;

AddTeamPage::AddTeamPage(Core::Config &config, Core::TeamsModel &team)
    : QWidget()
    , mConfig(config)
    , mTeams(teams)
{
    ui.setupUi(this);

    connect(ui.signInButton, &QPushButton::clicked,
            this, [this]() {
                mAuth = std::make_unique<Core::AuthController>();
                connect(mAuth.get(), &Core::AuthController::stateChanged,
                        this, &AddTeamPage::authStateChanged);
                mAuth->start();
            });
}

void AddTeamPage::authStateChanged(Core::AuthController::State state)
{
    switch (state) {
    case Core::AuthController::State::Error:
        setError(mAuth->error());
        break;
    case Core::AuthController::State::Done: {
        auto team = mAuth->team();
        auto controller = std::make_unique<Core::TeamController>(mConfig, std::move(team));
        Q_EMIT teamAdded(controller.get());
    }
    }
}
