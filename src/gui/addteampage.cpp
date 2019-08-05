#include "addteampage.h"
#include "core/config.h"

using namespace Gui;

AddTeamPage::AddTeamPage(Core::Config &config)
    : QWidget()
    , mConfig(config)
{
    ui.setupUi(this);
}
