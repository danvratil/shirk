#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QPointer>
#include <QJsonValue>
#include <QUrlQuery>

#include <memory>
#include <vector>
#include <functional>

#include "team.h"
#include "future.h"
#include "slackapi/common.h"
#include "utils/compat.h"

namespace Shirk::Core
{

class Config;

class NetworkDispatcher : public QObject
{
    Q_OBJECT

public:
    NetworkDispatcher(Config &config);
    ~NetworkDispatcher() override;

    template<typename Msg>
    Future sendRequest(Msg &&msg);

    template<typename Msg>
    Future sendRequest(const Team &team, Msg &&msg);

private:
    struct Request {
        SlackAPI::Method method;
        QUrl url;
        Promise<QJsonObject> promise;
    };

    Future<QJsonObject> enqueueRequest(SlackAPI::Method method, const QUrl &url);
    QUrl urlForEndpoint(QStringView endpoint, const QUrlQuery &query, std::optional<QString> token = std::nullopt) const;
    void tryDispatchNextRequest();
    void dispatchRequest(Request &&request);

    template<typename Msg>
    std::optional<QString> getTokenForMsg(const Team &team);

    int mMaxRunningRequests = 5;
    QNetworkAccessManager mNam;

    QVector<Request> mPendingRequests;
    std::vector<std::unique_ptr<QNetworkReply, DeleteLater>> mRunningRequests;
};


template<typename Msg>
Future<QJsonObject> NetworkDispatcher::sendRequest(Msg &&msg)
{
    using MsgT = std::decay_t<Msg>;
    return enqueueRequest(MsgT::method, urlForEndpoint(MsgT::endpoint, msg.serialize()));
}

template<typename Msg>
Future<QJsonObject> NetworkDispatcher::sendRequest(const Team &team, Msg &&msg)
{
    using MsgT = std::decay_t<Msg>;
    enqueueRequest(MsgT::method, urlForEndpoint(MsgT::endpoint, msg.serialize(), getTokenForMsg<Msg>(team)));
}

template<typename Msg>
std::optional<QString> NetworkDispatcher::getTokenForMsg(const Team &team)
{
    if constexpr (std::is_base_of_v<SlackAPI::UserAuthenticatedRequest, std::decay_t<Msg>>) {
        return team.accessToken();
    } else if constexpr (std::is_base_of_v<SlackAPI::BotAuthenticatedRequest, std::decay_t<Msg>>) {
        return team.botAccessToken();
    } else {
        return std::nullopt;
    }
}

} // namespace

