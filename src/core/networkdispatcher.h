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
#include "slackapi/common.h"
#include "utils/compat.h"

namespace Shirk::Core
{

class Config;

class NetworkDispatcher : public QObject
{
    Q_OBJECT

public:
    using ResponseCallback = std::function<void(const QJsonValue &)>;

    NetworkDispatcher(Config &config);
    ~NetworkDispatcher() override;

    template<typename Msg, typename Obj, typename Func>
    void sendRequest(Msg &&msg, Obj *obj, Func &&func);

    template<typename Msg, typename Obj, typename Func>
    void sendRequest(const Team &team, Msg &&msg, Obj *obj, Func &&func);

private:
    struct Request {
        SlackAPI::Method method;
        QUrl url;
        QPointer<QObject> obj;
        ResponseCallback func;
    };

    void enqueueRequest(SlackAPI::Method method, const QUrl &url, QObject *obj, ResponseCallback &&func);
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


template<typename Msg, typename Obj, typename Func>
void NetworkDispatcher::sendRequest(Msg &&msg, Obj *obj, Func &&func)
{
    using MsgT = std::decay_t<Msg>;
    const auto url = urlForEndpoint(MsgT::endpoint, msg.serialize());
    enqueueRequest(MsgT::method, url, obj, [obj, func = std::move(func)](const QJsonValue &val) mutable {
            if (obj) func(val);
    });
}

template<typename Msg, typename Obj, typename Func>
void NetworkDispatcher::sendRequest(const Team &team, Msg &&msg, Obj *obj, Func &&func)
{
    using MsgT = std::decay_t<Msg>;
    const auto url = urlForEndpoint(MsgT::endpoint, msg.serialize(), getTokenForMsg<Msg>(team));
    enqueueRequest(MsgT::method, url, obj, [obj, func = std::move(func)](const QJsonValue &val) mutable {
            if (obj) func(val);
    });
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

