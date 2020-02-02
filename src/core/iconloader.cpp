#include "iconloader.h"
#include "future.h"
#include "utils/stringliterals.h"
#include "core_debug.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QIcon>
#include <QStandardPaths>
#include <QDir>
#include <QGlobalStatic>
#include <QLoggingCategory>

#include <memory>

using namespace Shirk;
using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

Q_LOGGING_CATEGORY(LOG_ICONLOADER, "cz.dvratil.shirk.core.IconLoader")

Q_GLOBAL_STATIC(QNetworkAccessManager, sNAM)

Future<QIcon> IconLoader::load(const QUrl &url)
{
    if (const auto icon = loadFromCache(url); icon.has_value()) {
        return makeReadyFuture(*icon);
    } else {
        return fetchIcon(url).then([url](UniqueQObjectPtr<QNetworkReply> reply) {
            const auto icon = cacheIcon(url, reply->readAll());
            return icon;
        });
    }
}

Future<QIcon> IconLoader::load(const QVector<QUrl> &urls)
{
    struct SharedState {
        int runningTasks = 0;
        Promise<QIcon> promise;
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
                promise.setResult(result);
            }
        }
    };

    auto sharedState = std::make_shared<SharedState>(SharedState{urls.size(), Promise<QIcon>{}, QIcon{}});
    for (const auto &url : urls) {
        load(url).then([sharedState](const QIcon &icon) {
            sharedState->done(icon);
        });
    }
    return sharedState->promise.getFuture();

}


std::optional<QIcon> IconLoader::loadFromCache(const QUrl &url)
{
    const auto cachedir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (QDir{cachedir}.exists(url.fileName())) {
        QPixmap pixmap{cachedir + QDir::separator() + url.fileName()};
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }

    return std::nullopt;
}

Future<UniqueQObjectPtr<QNetworkReply>> IconLoader::fetchIcon(const QUrl &url)
{
    Promise<UniqueQObjectPtr<QNetworkReply>> promise;
    auto future = promise.getFuture();

    QNetworkRequest request(url);
    auto reply = sNAM->get(request);
    QObject::connect(reply, &QNetworkReply::finished,
        [promise = std::move(promise), reply = UniqueQObjectPtr<QNetworkReply>{reply}]() mutable {
            promise.setResult(std::move(reply));
        });
    return future;
}

QIcon IconLoader::cacheIcon(const QUrl &url, const QByteArray &data)
{
    const auto cachedir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (const auto dir = QDir{cachedir}; !dir.mkpath(cachedir)) {
        qCWarning(LOG_CORE) << "Failed to create cache directory" << cachedir;
    } else {
        QFile file(cachedir + QDir::separator() + url.fileName());
        if (!file.open(QIODevice::WriteOnly)) {
            qCWarning(LOG_CORE, "Failed to open cache file %s: %s", qUtf8Printable(file.fileName()),
                      qUtf8Printable(file.errorString()));
        } else {
            file.write(data);
        }
    }

    return QIcon{QPixmap{data}};
}

