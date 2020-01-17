#pragma once

#include <QIcon>

#include <optional>
#include <functional>

class QNetworkReply;


namespace Shirk::Core
{

class IconLoader
{
public:
    static void load(const QUrl &url, std::function<void(const QIcon &)> &&cb);

private:
    static std::optional<QIcon> loadFromCache(const QUrl &url);
    static void fetchIcon(const QUrl &url, std::function<void(QNetworkReply &)> &&cb);
    static QIcon cacheIcon(const QUrl &url, const QByteArray &data);
};

}
