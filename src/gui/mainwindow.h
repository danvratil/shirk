#pragma once

#include <QMainWindow>

#include "core/environment.h"
#include "core/teamsmodel.h"
#include "ui_mainwindow.h"

namespace Core {
class Environment;
class TeamController;
}

namespace Gui {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Core::Environment &environment, Core::TeamsModel &teams);

    void restoreFromConfig();

private:
    void initializePageStack();
    void openTeamPage(Core::TeamController &controller);
    void openAddTeamPage();

    Ui::MainWindow ui;
    Core::Environment &mEnv;
    Core::TeamsModel &mTeams;
};

}
