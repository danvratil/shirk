#include "networkdispatcher.h"
#include "core_debug.h"
#include "utils/stringliterals.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QUrlQuery>

#include <functional>

using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

NetworkDispatcher::NetworkDispatcher(Config &config)
{}

NetworkDispatcher::~NetworkDispatcher() = default;

Future<QJsonObject> NetworkDispatcher::enqueueRequest(SlackAPI::Method method, const QUrl &url)
{
    mPendingRequests.emplace_back(Request{method, url, Promise<QJsonObject>{}});
    tryDispatchNextRequest();
    return mPendingRequests.back().promise.getFuture();
}


void NetworkDispatcher::tryDispatchNextRequest()
{
    if (mRunningRequests.size() >= mMaxRunningRequests || mPendingRequests.empty()) {
        return;
    }

    dispatchRequest(std::move(mPendingRequests.front()));
    mPendingRequests.pop_front();
}

void NetworkDispatcher::dispatchRequest(Request &&data)
{
    QNetworkRequest request(data.url);
    std::unique_ptr<QNetworkReply, DeleteLater> reply(mNam.get(request));
    connect(reply.get(), &QNetworkReply::finished,
            this, [this, reply = reply.get(), request = std::move(data)]() mutable {
                const auto rawData = reply->readAll();
                const auto json = QJsonDocument::fromJson(rawData);
                if (!json[QStringLiteral("ok")].toBool()) {
                    request.promise.setError(u"Error response: %1"_qs.arg(QString::fromUtf8(rawData)));
                } else{
                    request.promise.setResult(json.object());
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
