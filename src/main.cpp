#include <QApplication>
#include <QDebug>
#include <KDBusService>

#include "core/config.h"
#include "core/teamsmodel.h"
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
    Core::TeamsModel teams(config);
    Gui::MainWindow window(config, teams);
    window.restoreFromConfig();

    QObject::connect(&uniqueApp, &KDBusService::activateRequested,
            [&window]() {
                qDebug() << "Activation requested";
                window.showNormal();
                window.raise();
                window.activateWindow();
            });

    return app.exec();
}
