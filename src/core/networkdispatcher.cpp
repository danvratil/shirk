#include "networkdispatcher.h"

using namespace Core;

NetworkDispatcher::NetworkDispatcher(Config &config)
{}

NetworkDispatcher::~NetworkDispatcher() = default;

void NetworkDispatcher::enqueueRequest(API::Method method, const QUrl &url, QObject *obj, ResponseCallback &&func)
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

QUrl NetworkDispatcher::urlForEndpoint(const QString &endpoint, const QUrlQuery &query, std::optional<QString> token) const
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
