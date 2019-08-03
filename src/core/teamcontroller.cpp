#include "teamcontroller.h"
#include "team.h"

using namespace Core;

TeamController::TeamController(Config &config, std::unique_ptr<Team> team)
    : mConfig(config)
    , mTeam(std::move(team))
{}

TeamController::~TeamController() = default;

Team *TeamController::team() const
{
    return mTeam.get();
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
