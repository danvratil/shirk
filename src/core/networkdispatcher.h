#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPointer>

#include <memory>
#include <vector>
#include <functional>

#include "team.h"
#include "api.h"

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

    template<typename Msg, typename Obj>
    void sendRequest(Msg &&msg, Obj *obj, ResponseCallback &&func);

    template<typename Msg, typename Obj>
    void sendRequest(Team *team, Msg &&msg, Obj *obj, ResponseCallback &&func);

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
    std::vector<std::unique_ptr<QNetworkReply>> mRunningRequests;
};


template<typename Msg, typename Obj>
void NetworkDispatcher::sendRequest(Msg &&msg, Obj *obj, ResponseCallback &&func)
{
    const auto url = urlForEndpoint(Msg::endpoint, msg.serialize());
    enqueueRequest(Msg::method, url, obj, std::move(func));
}

template<typename Msg, typename Obj>
void NetworkDispatcher::sendRequest(Team *team, Msg &&msg, Obj *obj, ResponseCallback &&func)
{
    const auto url = urlForEndpoint(Msg::endpoint, msg.serialize(), team->accessToken());
    enqueueRequest(Msg::method, url, obj, std::move(func));
}

}
