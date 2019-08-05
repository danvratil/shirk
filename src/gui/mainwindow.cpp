#include "mainwindow.h"

#include "core/config.h"
#include "core/teamsmodel.h"
#include "core/compat.h"

#include "addteampage.h"
#include "teampage.h"

using namespace Gui;

MainWindow::MainWindow(Core::Config &config, Core::TeamsModel &teams)
    : QMainWindow()
    , mConfig(config)
    , mTeams(teams)
{
    ui.setupUi(this);

    ui.teamListView->setModel(&teams);
    connect(ui.teamListView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, [this](const auto &index) {
                const auto team = mTeams.teamForIndex(index);
                Q_ASSERT(team);
                openTeamPage(*team);
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

void MainWindow::openTeamPage(Core::TeamController &team)
{
    auto page = findTeamPage(ui.stackedWidget, team);
    if (!page) {
        page = new TeamPage(team, mConfig);
        ui.stackedWidget->addWidget(page);
    }

    ui.stackedWidget->setCurrentWidget(page);
}

void MainWindow::openAddTeamPage()
{
    ui.addTeamAction->setEnabled(false);
    std::unique_ptr<AddTeamPage, DeleteLater> page(new AddTeamPage(mConfig));
    ui.stackedWidget->setCurrentIndex(ui.stackedWidget->addWidget(page.get()));
    connect(page.get(), &AddTeamPage::teamAdded,
            this, [this, page = std::move(page)](Core::TeamController *newTeam) {
                ui.stackedWidget->removeWidget(page.get());

                openTeamPage(*newTeam);
                ui.addTeamAction->setEnabled(true);

                page.reset();
            });
}
