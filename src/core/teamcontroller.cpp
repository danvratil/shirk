#include "teamcontroller.h"
#include "config.h"
#include "environment.h"
#include "team.h"
#include "conversationsmodel.h"
#include "usermanager.h"
#include "rtmcontroller.h"
#include "core_debug.h"
#include "future.h"
#include "networkdispatcher.h"

#include "utils/compat.h"
#include "slackapi/conversations.h"

#include <QMetaEnum>
#include <QMetaObject>

using namespace Shirk::Core;

QDebug operator<<(QDebug dbg, TeamController::Status status)
{
    const auto idx = TeamController::staticMetaObject.indexOfEnumerator("Status");
    Q_ASSERT(idx > -1);
    const auto enumerator = TeamController::staticMetaObject.enumerator(idx);
    return dbg.noquote() << enumerator.valueToKey(static_cast<int>(status));
}


TeamController::TeamController(Environment &environment, std::unique_ptr<Team> team)
    : mEnv(environment)
    , mTeam(std::move(team))
    , mUserManager{*mTeam.get(), mEnv}
    , mConversations{*this, mEnv}
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
        qCDebug(LOG_CORE) << "State changed to" << status;
        mStatus = status;
        Q_EMIT statusChanged(status);
    }
}

void TeamController::start()
{
    setStatus(Status::Connecting);

    FutureWatcher startWatcher;

    mRTMController = make_unique_qobject<RTMController>(*mTeam.get(), mEnv);
    mRTMController->start();
    Promise<void> rtmPromise;
    startWatcher(rtmPromise.getFuture());
    connect(mRTMController.get(), &RTMController::stateChanged,
            this, [this, rtmPromise = std::move(rtmPromise)](RTMController::State state) mutable {
                switch (state) {
                case RTMController::State::Connecting:
                    qCDebug(LOG_CORE) << "RTMController connecting ...";
                    break;
                case RTMController::State::Connected:
                    qCDebug(LOG_CORE) << "RTMController connected.";
                    rtmPromise.setResult();
                    break;
                case RTMController::State::Disconnected:
                    qCDebug(LOG_CORE) << "RTMController disconnected!";
                    mRTMController.reset();
                    break;
                }
            });
    startWatcher(mConversations.populate());
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
