#include "teamsmodel.h"
#include "teamcontroller.h"
#include "team.h"
#include "environment.h"
#include "config.h"
#include "core_debug.h"

Q_DECLARE_METATYPE(Shirk::Core::Team*)

using namespace Shirk::Core;

TeamsModel::~TeamsModel() = default;

void TeamsModel::loadControllers(Environment &env)
{
    beginResetModel();
    const auto teamIds = env.config.teamIds();
    for (const auto &teamId : teamIds) {
        qCDebug(LOG_CORE) << "Loading team" << teamId;
        auto config = env.config.settingsForTeam(teamId);
        mControllers.push_back(std::make_unique<TeamController>(env, Team::fromSettings(config.get())));
        mControllers.back()->start();
    }
    endResetModel();
}

QHash<int, QByteArray> TeamsModel::roleNames() const
{
    auto roles = QAbstractListModel::roleNames();
    roles[TeamRole] = "team";
    return roles;
}

int TeamsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mControllers.size();
}

QVariant TeamsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mControllers.size() || index.column() != 0) {
        return {};
    }

    const auto &controller = mControllers[index.row()];
    switch (role) {
        case Qt::DisplayRole:
            return controller->team().name();
        case Qt::DecorationRole:
            return controller->team().icon();
        case TeamRole:
            return QVariant::fromValue(&controller->team());
        default:
            return {};
    }
}

TeamController &TeamsModel::controllerForIndex(const QModelIndex &index) const
{
    Q_ASSERT(index.row() >= 0 && index.row() < mControllers.size());

    return *mControllers[index.row()].get();
}


TeamController &TeamsModel::controllerForTeam(const Team &team) const
{

    const auto ctrl = std::find_if(mControllers.cbegin(), mControllers.cend(),
                                  [&team](const auto &ctrl) {
                                      return ctrl->team().id() == team.id();
                                  });
    Q_ASSERT(ctrl != mControllers.cend());

    return *ctrl->get();
}

void TeamsModel::addTeam(std::unique_ptr<TeamController> team)
{
    beginInsertRows({}, mControllers.size(), mControllers.size());
    team->updateConfig();
    team->start();
    mControllers.push_back(std::move(team));
    endInsertRows();
}

