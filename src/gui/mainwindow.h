#pragma once

#include <QMainWindow>

#include "ui_mainwindow.h"

namespace Core {
class Config;
class TeamsModel;
class TeamController;
}

namespace Gui {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Core::Config &config, Core::TeamsModel &teams);

    void restoreFromConfig();

private:
    void initializePageStack();
    void openTeamPage(Core::TeamController &controller);
    void openAddTeamPage();

    Ui::MainWindow ui;
    Core::Config &mConfig;
    Core::TeamsModel &mTeams;
};

}
