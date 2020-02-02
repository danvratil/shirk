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
#include <qcryptographichash.h>

using namespace Shirk;
using namespace Shirk::Core;
using namespace Shirk::StringLiterals;

Q_LOGGING_CATEGORY(LOG_ICONLOADER, "cz.dvratil.shirk.core.IconLoader", QtMsgType::QtInfoMsg)

Q_GLOBAL_STATIC(QNetworkAccessManager, sNAM)

Future<QIcon> IconLoader::load(const QUrl &url)
{
    if (!url.isValid()) {
        return makeReadyFuture(QIcon{});
    }

    if (const auto icon = loadFromCache(url); icon.has_value()) {
        qCDebug(LOG_ICONLOADER) << "Found icon" << url << "in cache.";
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
    Q_ASSERT(url.isValid());
    const auto name = nameFromUrl(url);
    const auto cachedir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (QDir{cachedir}.exists(name)) {
        QPixmap pixmap{cachedir + QDir::separator() + name};
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }

    return std::nullopt;
}

Future<UniqueQObjectPtr<QNetworkReply>> IconLoader::fetchIcon(const QUrl &url)
{
    Q_ASSERT(url.isValid());

    Promise<UniqueQObjectPtr<QNetworkReply>> promise;
    auto future = promise.getFuture();

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto reply = sNAM->get(request);
    QObject::connect(reply, &QNetworkReply::finished,
        [promise = std::move(promise), reply = UniqueQObjectPtr<QNetworkReply>{reply}]() mutable {
            promise.setResult(std::move(reply));
        });
    return future;
}

QIcon IconLoader::cacheIcon(const QUrl &url, const QByteArray &data)
{
    Q_ASSERT(url.isValid());

    const auto cachedir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (const auto dir = QDir{cachedir}; !dir.mkpath(cachedir)) {
        qCWarning(LOG_ICONLOADER) << "Failed to create cache directory" << cachedir;
    } else {
        const auto name = nameFromUrl(url);
        QFile file{cachedir + QDir::separator() + name};
        if (!file.open(QIODevice::WriteOnly)) {
            qCWarning(LOG_ICONLOADER, "Failed to open cache file %s: %s", qUtf8Printable(file.fileName()),
                      qUtf8Printable(file.errorString()));
        } else {
            file.write(data);
            qCDebug(LOG_ICONLOADER) << "Added icon" << url << "to local cache as" << name;
        }
    }

    QPixmap pixmap;
    pixmap.loadFromData(data);
    return {pixmap};
}

QString IconLoader::nameFromUrl(const QUrl &url)
{
    return QString::fromLatin1(
            QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Md5).toHex())
           + u".png"_qs;
}
