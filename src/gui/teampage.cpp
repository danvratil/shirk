#include "teampage.h"
#include "core/environment.h"

using namespace Gui;

TeamPage::TeamPage(Core::TeamController &team, Core::Environment &environment)
    : QWidget()
    , mTeam(team)
    , mEnv(environment)
{}


