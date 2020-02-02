#pragma once

#include <QObject>

#include <memory>

#include "rtmsocket.h"
#include "slackapi/events.h"

namespace Shirk::Core
{

class Team;
struct Environment;
class RTMEventListener;

class RTMController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(RTMController::State state READ state NOTIFY stateChanged)
public:
    RTMController(const Team &team, Environment &env);
    ~RTMController() override;

    void start();
    void stop();

    enum class State {
        Disconnected,
        Connecting,
        Connected
    };
    State state() const {
        return mState;
    }

    void subscribeListener(RTMEventListener *listener, SlackAPI::RTM::EventType event);
    void unsubscribeListener(RTMEventListener *listener, SlackAPI::RTM::EventType event);
    void unsubscribeListener(RTMEventListener *listener);
Q_SIGNALS:
    void stateChanged(State state);
    void error(const QString &error);
    void connectionLost();

private:
    void setState(State newState);
    void eventsAvailable();

    const Team &mTeam;
    const Environment &mEnv;
    State mState = State::Disconnected;

    RTMSocketPtr mSocket;

    std::array<std::vector<RTMEventListener *>, static_cast<std::size_t>(SlackAPI::RTM::EventType::_EventCount)> mSubscribers;
};


} // namespace
