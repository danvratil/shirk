#include "authcontroller.h"
#include "networkdispatcher.h"
#include "shirk_config.h"
#include "environment.h"
#include "core_debug.h"
#include "slackapi/oauth.h"
#include "slackapi/teaminfo.h"
#include "utils/compat.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QUuid>

using namespace Shirk::Core;

AuthController::AuthController(Environment &environment)
    : mEnv(environment)
{}

AuthController::~AuthController() = default;

void AuthController::start()
{
    startServer();
    launchBrowser();
    waitForCode([this](const QString &code) mutable {
        exchangeCodeForToken(code, [this](const SlackAPI::OAuthAccessResponse &resp) mutable {
            shutdownServer();

            mTeam = std::make_unique<Team>(resp.teamId, resp.teamName, resp.accessToken);
            fetchTeamInfo();
        });
    });
}

void AuthController::setState(State state)
{
    if (mState != state) {
        mState = state;
        Q_EMIT stateChanged(state);
    }
}

void AuthController::setError(const QString &error)
{
    mError = error;
    setState(State::Error);
}

void AuthController::startServer()
{
    static constexpr const uint16_t port = 44916;

    mServer = std::make_unique<QTcpServer>();
    if (mServer->listen(QHostAddress::Any, port)) {
        setError(tr("Failed to setup authentication flow."));
        return;
    }
    connect(mServer.get(), &QTcpServer::acceptError,
            this, [this](QAbstractSocket::SocketError error) {
                setError(tr("Error during authentication: %1").arg(mServer->errorString()));
            });
}

void AuthController::launchBrowser()
{
    Q_ASSERT(mServer);

    mAuthState = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QUrl url(QStringLiteral("https://slack.com/oauth/authorize"));
    QUrlQuery query(url);
    query.addQueryItem(QStringLiteral("client_id"), clientId);
    query.addQueryItem(QStringLiteral("scope"), scopes.join(QLatin1Char(' ')));
    query.addQueryItem(QStringLiteral("redirect_uri"), QStringLiteral("http://127.0.0.1:%1/").arg(mServer->serverPort()));
    query.addQueryItem(QStringLiteral("state"), mAuthState);
    url.setQuery(query);

    if (!QDesktopServices::openUrl(url)) {
        setError(tr("Failed to launch browser."));
        return;
    }

    setState(State::WaitingForBrowser);
}

void AuthController::waitForCode(std::function<void(const QString &)> &&cb)
{
    Q_ASSERT(mServer);
    connect(mServer.get(), &QTcpServer::newConnection,
            this, [this, cb = std::move(cb)]() mutable {
                std::unique_ptr<QTcpSocket, DeleteLater> socket(mServer->nextPendingConnection());
                readFromSocket(std::move(socket), std::move(cb));
            });
}

namespace
{
struct Result {
    QString code;
    QString state;
};
std::optional<Result> parseCode(const QByteArray &data)
{
    const auto line = data.split(' ');
    if (line.size() != 3 || line.at(0) != QByteArray("GET") || !line.at(2).startsWith(QByteArray("HTTP/1.1"))) {
        return std::nullopt;
    }

    const QUrl url(QString::fromLatin1(line.at(1)));
    const QUrlQuery query(url);
    return Result{query.queryItemValue(QStringLiteral("code")), query.queryItemValue(QStringLiteral("state"))};
}
}

void AuthController::readFromSocket(std::unique_ptr<QTcpSocket, DeleteLater> socket, std::function<void(const QString &)> &&cb)
{
    connect(socket.get(), &QTcpSocket::readyRead,
            this, [this, socket = std::move(socket), cb = std::move(cb)]() mutable {
                const auto data = socket->readLine();
                socket->write("HTTP/1.1 200 OK\n");
                socket->flush();
                socket.reset();

                const auto result = parseCode(data);
                if (!result.has_value()) {
                    setError(tr("Failed to receive authentication code from Slack."));
                } else if (result->state != mAuthState) {
                    setError(tr("Security code mismatch."));
                } else {
                    cb(result->code);
                }
            });
}

void AuthController::exchangeCodeForToken(const QString &code, std::function<void(const SlackAPI::OAuthAccessResponse&)> &&cb)
{
    setState(State::RetrievingToken);
    mEnv.networkDispatcher.sendRequest(SlackAPI::OAuthAccessRequest{{}, clientId, clientSecret, code,
            QStringLiteral("http://127.0.0.1:%1/").arg(mServer->serverPort())},
                            this, [cb = std::move(cb)](const auto &data) {
                                cb(SlackAPI::OAuthAccessResponse::parse(data));
                            });
}

void AuthController::fetchTeamInfo()
{
    setState(State::RetrievingTeamInfo);
    mEnv.networkDispatcher.sendRequest(mTeam.get(),
                            SlackAPI::TeamInfoRequest{{}, mTeam->id()},
                            this, [this](const auto &data) mutable {
                                mTeam->updateFromTeamInfo(SlackAPI::TeamInfoResponse::parse(data));
                                setState(State::Done);
                            });
}

void AuthController::shutdownServer()
{
    Q_ASSERT(mServer);
    mServer->close();
    mServer.reset();
}

QDebug operator<<(QDebug debug, AuthController::State state)
{
    auto dbg = debug.noquote();
    switch (state) {
    case AuthController::State::None:
        return dbg << "AuthController::State::None";
    case AuthController::State::Done:
        return dbg << "AuthController::State::Done";
    case AuthController::State::Error:
        return dbg << "AuthController::State::Error";
    case AuthController::State::RetrievingTeamInfo:
        return dbg << "AuthController::State::RetrievingTeamInfo";
    case AuthController::State::RetrievingToken:
        return dbg << "AuthController::State::RetrievingToken";
    case AuthController::State::WaitingForBrowser:
        return dbg << "AuthController::State::WaitingForBrowser";
    }

    Q_UNREACHABLE();
}

