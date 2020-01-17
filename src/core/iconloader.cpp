#include "iconloader.h"

#include <QNetworkReply>

using namespace Shirk::Core;

void IconLoader::load(const QUrl &url, std::function<void(const QIcon &)> &&callback)
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


