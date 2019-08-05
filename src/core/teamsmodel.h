#pragma once

#include <QAbstractListModel>

#include <memory>

namespace Core {

class Config;
class TeamController;

class TeamsModel : public QAbstractListModel
{
public:
    enum Roles {
        TeamRole = Qt::UserRole + 1,
    };

    TeamsModel(Config &config);
    ~TeamsModel() override;

    void loadControllers();

    TeamController *teamForIndex(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    Config &mConfig;
    std::vector<std::unique_ptr<TeamController>> mControllers;

};

} // namespace
