#pragma once

#include <QMainWindow>

namespace Core {
class Config;
class TeamsModel;
}

namespace Gui {

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Core::Config &config, Core::TeamsModel &teams);

    void restoreFromConfig();

private:
    Core::Config &mConfig;
    Core::TeamsModel &mTeams;
};

}
