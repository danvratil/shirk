#include "teamsmodel.h"
#include "teamcontroller.h"
#include "team.h"

Q_DECLARE_METATYPE(Core::Team*)

using namespace Core;

TeamsModel::TeamsModel(Config &config)
    : mConfig(config)
{}

TeamsModel::~TeamsModel() = default;

void TeamsModel::loadControllers()
{

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
            return controller->team()->name();
        case Qt::DecorationRole:
            return controller->team()->icon();
        case TeamRole:
            return QVariant::fromValue(controller->team());
        default:
            return {};
    }
}

TeamController *TeamsModel::teamForIndex(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() >= mControllers.size()) {
        return nullptr;
    }

    return mControllers[index.row()].get();
}
