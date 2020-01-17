#include "teamcontroller.h"
#include "config.h"
#include "environment.h"
#include "team.h"

using namespace Shirk::Core;

TeamController::TeamController(Environment &environment, std::unique_ptr<Team> team)
    : mEnv(environment)
    , mTeam(std::move(team))
{}

TeamController::~TeamController() = default;

const Team &TeamController::team() const
{
    return *mTeam.get();
}

Team &TeamController::team()
{
    return *mTeam.get();
}

TeamController::Status TeamController::status() const
{
    return mStatus;
}

void TeamController::setStatus(Status status)
{
    if (mStatus != status) {
        mStatus = status;
        Q_EMIT statusChanged(status);
    }
}

void TeamController::start()
{
    setStatus(Status::Connecting);
    // TODO
}

void TeamController::quit()
{
    // TODO
    setStatus(Status::Disconnected);
}

void TeamController::updateConfig()
{
    auto settings = mEnv.config.settingsForTeam(mTeam->id());
    mTeam->updateConfig(settings.get());
}
