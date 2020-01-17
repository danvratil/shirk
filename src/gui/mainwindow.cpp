#include "mainwindow.h"

#include "core/config.h"
#include "core/environment.h"
#include "core/teamsmodel.h"
#include "core/teamcontroller.h"
#include "utils/compat.h"

#include "addteampage.h"
#include "teampage.h"

using namespace Shirk;
using namespace Shirk::Gui;

MainWindow::MainWindow(Core::Environment &environment, Core::TeamsModel &teams)
    : QMainWindow()
    , mEnv(environment)
    , mTeams(teams)
{
    ui.setupUi(this);

    ui.teamListView->setModel(&mTeams);
    connect(ui.teamListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this](const auto &index) {
                openTeamPage(mTeams.controllerForIndex(index));
            });
    connect(ui.addTeamAction, &QAction::triggered, this, &MainWindow::openAddTeamPage);
}

void MainWindow::restoreFromConfig()
{
    // TODO: Restore window position and size from configuration
    // TODO: Start minimized?
    showNormal();
}

namespace {

TeamPage *findTeamPage(QStackedWidget *pages, Core::TeamController &team)
{
    for (int i = 0; i < pages->count(); ++i) {
        const auto page = qobject_cast<TeamPage*>(pages->widget(i));
        if (page && &page->team() == &team) {
            return page;
        }
    }
    return nullptr;
}

}

void MainWindow::openTeamPage(Core::TeamController &controller)
{
    auto page = findTeamPage(ui.stackedWidget, controller);
    if (!page) {
        page = new TeamPage(controller, mEnv);
        ui.stackedWidget->addWidget(page);
    }

    ui.stackedWidget->setCurrentWidget(page);
}

void MainWindow::openAddTeamPage()
{
    ui.addTeamAction->setEnabled(false);
    auto page = make_unique_qobject<AddTeamPage>(mTeams, mEnv);
    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->addWidget(page.get()));
    connect(page.get(), &AddTeamPage::teamAdded,
            this, [this, page = std::move(page)](Core::Team &newTeam) mutable {
                ui.stackedWidget->removeWidget(page.get());
                openTeamPage(mTeams.controllerForTeam(newTeam));
                ui.addTeamAction->setEnabled(true);

                page.reset();
            });
}
