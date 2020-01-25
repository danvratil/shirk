#include "rtmcontroller.h"
#include "environment.h"
#include "networkdispatcher.h"
#include "slackapi/rtm.h"
#include "slackapi/events.h"
#include "utils/stringliterals.h"

#include <QLoggingCategory>
#include <QThread>

Q_LOGGING_CATEGORY(LOG_RTM, "cz.dvratil.shirk.rtm")

using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

QDebug operator<<(QDebug dbg, RTMController::State state)
{
    switch (state) {
    case RTMController::State::Connected:
        return dbg.noquote() << "Connected";
    case RTMController::State::Connecting:
        return dbg.noquote() << "Connecting";
    case RTMController::State::Disconnected:
        return dbg.noquote() << "Disconnected";
    }

    Q_UNREACHABLE();
}

RTMController::RTMController(const Team &team, Environment &env)
    : mTeam(team)
    , mEnv(env)
{}

RTMController::~RTMController() = default;

void RTMController::setState(State state)
{
    if (mState != state) {
        qCDebug(LOG_RTM) << "State changed to " << state;
        mState = state;
        Q_EMIT stateChanged(mState);
    }
}

void RTMController::start()
{
    setState(State::Connecting);

    mEnv.networkDispatcher.sendRequest(
        mTeam, SlackAPI::RTMConnectRequest{},
        this, [this](const QJsonValue &data) {
            const auto resp = SlackAPI::RTMConnectResponse::parse(data);
            if (resp.url.isValid()) {
                mSocket = RTMSocketPtr{new RTMSocket{resp.url}};
                connect(mSocket.get(), &RTMSocket::connected, this,
                        [this]() {
                            setState(State::Connected);
                        });
                connect(mSocket.get(), &RTMSocket::connectionLost, this,
                        [this]() {
                            setState(State::Disconnected);
                            Q_EMIT connectionLost();
                        });
                connect(mSocket.get(), &RTMSocket::eventsAvailable,
                        this, &RTMController::eventsAvailable);
            } else {
                Q_EMIT error(u"Failed to retrieve RTM auth URL"_qs);
            }
        });
}

void RTMController::stop()
{
    if (mSocket) {
        mSocket->disconnect();
    }
}

void RTMController::eventsAvailable()
{
    const auto event = mSocket->retrieveEvent();
    if (!event) {
        return;
    }

    // TODO: MATCH
}