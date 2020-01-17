#include "teampage.h"
#include "core/environment.h"

using namespace Shirk;
using namespace Shirk::Gui;

TeamPage::TeamPage(Core::TeamController &team, Core::Environment &environment)
    : QWidget()
    , mTeam(team)
    , mEnv(environment)
{}


