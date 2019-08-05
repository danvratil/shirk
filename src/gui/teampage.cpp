#include "teampage.h"

using namespace Gui;

TeamPage::TeamPage(Core::TeamController &team, Core::Config &config)
    : QWidget()
    , mTeam(team)
    , mConfig(config)
{}


