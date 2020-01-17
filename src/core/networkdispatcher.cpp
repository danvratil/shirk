#include "networkdispatcher.h"
#include "core_debug.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QUrlQuery>

#include <functional>

using namespace Shirk::Core;

NetworkDispatcher::NetworkDispatcher(Config &config)
{}

NetworkDispatcher::~NetworkDispatcher() = default;

void NetworkDispatcher::enqueueRequest(SlackAPI::Method method, const QUrl &url, QObject *obj, ResponseCallback &&func)
{
    mPendingRequests.push_back({method, url, obj, std::move(func)});
    tryDispatchNextRequest();
}


void NetworkDispatcher::tryDispatchNextRequest()
{
    if (mRunningRequests.size() >= mMaxRunningRequests || mPendingRequests.empty()) {
        return;
    }

    dispatchRequest(mPendingRequests.takeFirst());
}

void NetworkDispatcher::dispatchRequest(Request &&data)
{
    QNetworkRequest request(data.url);
    std::unique_ptr<QNetworkReply, DeleteLater> reply(mNam.get(request));
    connect(reply.get(), &QNetworkReply::finished,
            this, [this, reply = reply.get(), request = std::move(data)]() {
                const auto rawData = reply->readAll();
                const auto json = QJsonDocument::fromJson(rawData);
                if (!json[QStringLiteral("ok")].toBool()) {
                    // TODO: Error handling?!?!?
                    qCWarning(LOG_CORE) << "Error response:" << rawData;
                } else {
                    request.func(json.object());
                }

                auto it = std::find_if(mRunningRequests.begin(), mRunningRequests.end(),
                                       [reply](const auto &r) { return r.get() == reply; });
                Q_ASSERT(it != mRunningRequests.end());
                mRunningRequests.erase(it);

                tryDispatchNextRequest();
            });
    mRunningRequests.push_back(std::move(reply));
}

QUrl NetworkDispatcher::urlForEndpoint(QStringView endpoint, const QUrlQuery &query, std::optional<QString> token) const
{
    QUrl url(QStringLiteral("https://slack.com/api/%1").arg(endpoint));
    if (token.has_value()) {
        QUrlQuery q(query);
        q.addQueryItem(QStringLiteral("token"), *token);
        url.setQuery(q);
    } else {
        url.setQuery(query);
    }

    return url;
}
