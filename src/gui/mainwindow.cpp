#include "mainwindow.h"

#include "core/config.h"
#include "core/teamsmodel.h"

using namespace Gui;

MainWindow::MainWindow(Core::Config &config, Core::TeamsModel &teams)
    : QMainWindow()
    , mConfig(config)
    , mTeams(teams)
{}

void MainWindow::restoreFromConfig()
{
    // TODO: Restore window position and size from configuration
    // TODO: Start minimized?
    showNormal();
}
