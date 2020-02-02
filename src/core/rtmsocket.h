#pragma once

#include <QObject>
#include <QWebSocket>

#include <memory>
#include <deque>
#include <mutex>

namespace Shirk::SlackAPI::RTM
{
struct Event;
}

namespace Shirk::Core
{

class RTMSocket : public QObject
{
    Q_OBJECT
public:
    RTMSocket(const QUrl &url);
    ~RTMSocket() override;

    void quit();

    std::unique_ptr<SlackAPI::RTM::Event> retrieveEvent();

Q_SIGNALS:
    void eventsAvailable();
    void connected();
    void connectionLost();

    void error(const QString &error);

private:
    QWebSocket mSocket;
    std::deque<std::unique_ptr<SlackAPI::RTM::Event>> mEventQueue;
    std::mutex mQueueLock;
};


struct RTMSocketDeleter {
    void operator()(RTMSocket *socket);
};
using RTMSocketPtr = std::unique_ptr<RTMSocket, RTMSocketDeleter>;

} // namespace
