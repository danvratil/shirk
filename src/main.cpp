#include <QApplication>
#include <QDebug>
#include <KDBusService>

#include "core/config.h"
#include "core/environment.h"
#include "core/teamsmodel.h"
#include "core/networkdispatcher.h"
#include "gui/mainwindow.h"

#include "shirk_version.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
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

    Core::TeamsModel teams;
    Gui::MainWindow window(environment, teams);
    window.restoreFromConfig();

    QObject::connect(&uniqueApp, &KDBusService::activateRequested,
            [&window]() {
                qDebug() << "Activation requested";
                window.showNormal();
                window.raise();
                window.activateWindow();
            });

    // Delayed invocation once the app is running
    QMetaObject::invokeMethod(&app, [&environment, &teams]() {
        teams.loadControllers(environment);
    }, Qt::QueuedConnection);

    return app.exec();
}
