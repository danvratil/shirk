#pragma once

#include "future.h"
#include "utils/memory.h"

#include <QIcon>

#include <optional>
#include <functional>

class QNetworkReply;


namespace Shirk::Core
{

class IconLoader
{
public:
    static Future<QIcon> load(const QUrl &url);
    static Future<QIcon> load(const QVector<QUrl> &urls);

private:
    static std::optional<QIcon> loadFromCache(const QUrl &url);
    static Future<UniqueQObjectPtr<QNetworkReply>> fetchIcon(const QUrl &url);
    static QIcon cacheIcon(const QUrl &url, const QByteArray &data);
};

}
