#include "iconloader.h"

#include <QNetworkReply>
#include <QIcon>

#include <memory>

using namespace Shirk::Core;

void IconLoader::load(const QUrl &url, Callback &&callback)
{
    if (const auto icon = loadFromCache(url); icon.has_value()) {
        callback(*icon);
    } else {
        fetchIcon(url, [url, cb = std::move(callback)](QNetworkReply &reply) {
            const auto icon = cacheIcon(url, reply.readAll());
            cb(icon);
        });
    }
}

void IconLoader::load(const QVector<QUrl> &urls, Callback &&callback)
{
    struct SharedState {
        int runningTasks = 0;
        Callback callback;
        QIcon result;

        void done(const QIcon &icon) {
            if (result.isNull()) {
                result = icon;
            } else if (!icon.isNull()) {
                const auto sizes = icon.availableSizes();
                Q_ASSERT(sizes.size() == 1);
                result.addPixmap(icon.pixmap(sizes.at(0)));
            }

            --runningTasks;
            if (runningTasks == 0) {
                callback(result);
            }
        }
    };

    auto sharedState = std::make_shared<SharedState>(SharedState{urls.size(), std::move(callback), QIcon{}});
    for (const auto &url : urls) {
        load(url, [sharedState](const QIcon &icon) {
            sharedState->done(icon);
        });
    }

}


std::optional<QIcon> IconLoader::loadFromCache(const QUrl &url)
{
    // TODO
    return std::nullopt;
}

void IconLoader::fetchIcon(const QUrl &url, std::function<void(QNetworkReply &)> &&callback)
{
    // TODO
}

QIcon IconLoader::cacheIcon(const QUrl &url, const QByteArray &data)
{
    // TODO
    return {};
}


