#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QDebug>

#include <KDBusService>

#include "core/config.h"
#include "core/environment.h"
#include "core/teamsmodel.h"
#include "core/networkdispatcher.h"
//#include "gui/mainwindow.h"
#include "utils/stringliterals.h"

#include <KPackage/PackageLoader>
#include <kpackage/packageloader.h>

#include "shirk_version.h"

#include <iostream>

using namespace Shirk;
using namespace Shirk::StringLiterals;

bool loadPackageUI(QQmlApplicationEngine &engine, const QString &packageName)
{
    const auto prefix = u"cz.dvratil.shirk.uipackages."_qs;
    const auto serviceType = u"Shirk/UiPackage"_qs;
    const auto packageLoader = KPackage::PackageLoader::self();

    const auto package = packageLoader->loadPackage(serviceType, prefix + packageName);
    if (!package.isValid()) {
        qCritical() << "Error loading UI package: " << packageName << "is not a valid package";
        qDebug() << "Available Qt Quick Ui packages for Shirk:";
        for (const auto pkg : packageLoader->listPackages(serviceType)) {
            qDebug() << "\t" << pkg.name() << "/" << pkg.pluginId();
        }
        return false;
    }

    engine.load(QUrl::fromLocalFile(package.filePath("window")));

    return true;
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Shirk"));
    app.setApplicationVersion(QStringLiteral(SHIRK_VERSION_STRING));
    app.setApplicationDisplayName(app.tr("Shirk"));
    app.setOrganizationName(app.tr("Daniel Vrátil"));
    app.setOrganizationDomain(QStringLiteral("dvratil.cz"));

    KDBusService uniqueApp(KDBusService::Unique);

    Core::Config config;
    Core::NetworkDispatcher networkDispatcher{config};

    Core::Environment environment{
        config,
        networkDispatcher
    };

    QQmlApplicationEngine engine;
    if (!loadPackageUI(engine, u"default"_qs)) {
        return 1;
    }

    Core::TeamsModel teams;
/*
    Gui::MainWindow window(environment, teams);
    window.restoreFromConfig();

    QObject::connect(&uniqueApp, &KDBusService::activateRequested,
            [&window]() {
                qDebug() << "Activation requested";
                window.showNormal();
                window.raise();
                window.activateWindow();
            });
*/
    // Delayed invocation once the app is running
    QMetaObject::invokeMethod(&app, [&environment, &teams]() {
        teams.loadControllers(environment);
    }, Qt::QueuedConnection);

    return app.exec();
}
