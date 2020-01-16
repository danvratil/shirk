#pragma once

#include <QObject>
#include <QDebug>

#include <functional>
#include <optional>
#include <memory>

#include "compat.h"
#include "team.h"

class QTcpServer;
class QTcpSocket;

namespace Core {

class Environment;
namespace API {
class OAuthAccessResponse;
}

class AuthController : public QObject
{
    Q_OBJECT
public:
    using Token = QString;
    using Error = QString;

    enum class State {
        None,
        WaitingForBrowser,
        RetrievingToken,
        RetrievingTeamInfo,
        Done,
        Error
    };

    AuthController(Environment &environment);
    ~AuthController();

    void start();

    State state() const;

    // TODO: Sucks, have start() take a callback maybe?
    std::unique_ptr<Team> team() { return std::move(mTeam); };
    Error error() const { return mError; }

Q_SIGNALS:
    void stateChanged(State state);


private:
    Q_DISABLE_COPY_MOVE(AuthController)

    QString generateState();
    void setState(State state);
    void setError(const Error &error);
    void startServer();
    void shutdownServer();
    void launchBrowser();
    void waitForCode(std::function<void(const QString &)> &&cb);
    void exchangeCodeForToken(const QString &code, std::function<void(const API::OAuthAccessResponse &)> &&cb);
    void readFromSocket(std::unique_ptr<QTcpSocket, DeleteLater> socket, std::function<void(const QString &)> &&cb);
    void fetchTeamInfo();

    State mState = State::None;
    std::unique_ptr<Team> mTeam;
    Error mError;
    QString mAuthState;

    std::unique_ptr<QTcpServer> mServer;

    Environment &mEnv;
};

}

QDebug operator<<(QDebug, Core::AuthController::State);
