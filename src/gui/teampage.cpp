#include "teampage.h"
#include "core/team.h"
#include "core/teamcontroller.h"
#include "core/environment.h"

using namespace Shirk;
using namespace Shirk::Gui;

TeamPage::TeamPage(Core::TeamController &team, Core::Environment &environment)
    : QWidget()
    , mTeam(team)
    , mEnv(environment)
{
    ui.setupUi(this);
    ui.conversationsTreeView->setModel(&team.conversations());
    ui.teamIcon->setPixmap(team.team().icon().pixmap(48));
    ui.teamLabel->setText(team.team().name());
}


