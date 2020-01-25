#pragma once

#include <QObject>

#include <memory>

#include "rtmsocket.h"

namespace Shirk::Core
{

class Team;
class Environment;
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
    State state() const { return mState; }

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
};


} // namespace
