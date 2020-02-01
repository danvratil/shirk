#pragma once

#include "conversation.h"
#include "future.h"

#include <QAbstractItemModel>

namespace Shirk::Core
{

class TeamController;
class Environment;

class ConversationsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ConversationsModel(TeamController &controller, Environment &environment);

    Future<void> populate();

    enum class RowType {
        Header,
        Conversation
    };

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    TeamController &mController;
    Environment &mEnvironment;

    struct Group {
        explicit Group(const QString &name): name(name) {}

        QString name;
        std::vector<std::unique_ptr<Conversation>> conversations;
    };

    std::vector<std::unique_ptr<Group>> mGroups;
};

} // namespace
