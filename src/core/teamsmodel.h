#pragma once

#include "core/teamcontroller.h"

#include <QAbstractListModel>

#include <memory>

namespace Shirk::Core {

class Environment;

class TeamsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TeamRole = Qt::UserRole + 1,
    };

    ~TeamsModel() override;

    void loadControllers(Environment &enviroment);

    TeamController &controllerForIndex(const QModelIndex &index) const;
    TeamController &controllerForTeam(const Team &team) const;
    void addTeam(std::unique_ptr<TeamController> team);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    std::vector<std::unique_ptr<TeamController>> mControllers;

};

} // namespace
