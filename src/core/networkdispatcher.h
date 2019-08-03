#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QPointer>
#include <QJsonValue>

#include <memory>
#include <vector>
#include <functional>

#include "team.h"
#include "api.h"
#include "compat.h"

namespace Core
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
    void sendRequest(Team *team, Msg &&msg, Obj *obj, Func &&func);

private:
    struct Request {
        API::Method method;
        QUrl url;
        QPointer<QObject> obj;
        ResponseCallback func;
    };

    void enqueueRequest(API::Method method, const QUrl &url, QObject *obj, ResponseCallback &&func);
    QUrl urlForEndpoint(const QString &endpoint, const QUrlQuery &query, std::optional<QString> token = std::nullopt) const;
    void tryDispatchNextRequest();
    void dispatchRequest(Request &&request);

    int mMaxRunningRequests = 5;
    QNetworkAccessManager mNam;

    QVector<Request> mPendingRequests;
    std::vector<std::unique_ptr<QNetworkReply, DeleteLater>> mRunningRequests;
};


template<typename Msg, typename Obj, typename Func>
void NetworkDispatcher::sendRequest(Msg &&msg, Obj *obj, Func &&func)
{
    sendRequest(nullptr, std::move(msg), obj, std::move(func));
}

template<typename Msg, typename Obj, typename Func>
void NetworkDispatcher::sendRequest(Team *team, Msg &&msg, Obj *obj, Func &&func)
{
    const auto url = urlForEndpoint(Msg::endpoint, msg.serialize(), team ? std::optional<QString>(team->accessToken()) : std::nullopt);
    enqueueRequest(Msg::method, url, obj, [obj, func = std::move(func)](const QJsonValue &val) mutable {
            if (obj) func(val);
    });
}

}

