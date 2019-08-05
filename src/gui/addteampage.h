#pragma once

#include <QWidget>

#include "ui_addteampage.h"

namespace Core {
class Config;
class TeamController;
}

namespace Gui
{

class AddTeamPage : public QWidget
{
    Q_OBJECT
public:
    explicit AddTeamPage(Core::Config &mConfig);

Q_SIGNALS:
    void teamAdded(Core::TeamController *);

private:
    Ui::AddTeamPage ui;
    Core::Config &mConfig;
};

}
