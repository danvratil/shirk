#include "rtmsocket.h"
#include "slackapi/events.h"
#include "utils/stringliterals.h"

#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

using namespace Shirk;
using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

void RTMSocketDeleter::operator()(RTMSocket *socket)
{
    socket->deleteLater();
    socket->thread()->quit();
    socket->thread()->wait();
    socket->thread()->deleteLater();
}

RTMSocket::RTMSocket(const QUrl &url)
{
    connect(&mSocket, &QWebSocket::textMessageReceived, this,
            [this](const QString &message) {
                QJsonParseError err;
                const auto doc = QJsonDocument::fromJson(message.toUtf8(), &err);
                if (err.error != QJsonParseError::NoError) {
                    Q_EMIT error(u"Failed to parse RTM JSON response: %1"_qs.arg(err.errorString()));
                    return;
                }

                try {
                    std::lock_guard lock{mQueueLock};
                    mEventQueue.push_back(SlackAPI::RTM::parseEvent(doc.object()));
                    Q_EMIT eventsAvailable();
                } catch (const SlackAPI::RTM::EventException &e) {
                    Q_EMIT error(u"Error when parsing RTM event: %1"_qs.arg(e.what()));
                }
            });
    connect(&mSocket, &QWebSocket::stateChanged, this,
            [this](QAbstractSocket::SocketState state) {
                switch (state) {
                case QAbstractSocket::ConnectedState:
                    Q_EMIT connected();
                    break;
                case QAbstractSocket::UnconnectedState:
                    Q_EMIT connectionLost();
                    break;
                default:
                    break;
                }
            });
    mSocket.open(url);
}


RTMSocket::~RTMSocket() = default;

void RTMSocket::quit()
{
    mSocket.close();
}

std::unique_ptr<SlackAPI::RTM::Event> RTMSocket::retrieveEvent()
{
    std::lock_guard lock{mQueueLock};
    auto event = std::move(mEventQueue.front());
    mEventQueue.pop_front();

    return event;
}
