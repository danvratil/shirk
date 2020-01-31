#include "conversationsmodel.h"
#include "teamcontroller.h"
#include "environment.h"

using namespace Shirk::Core;

namespace {

inline bool isRoot(const QModelIndex &index) {
    return !index.isValid();
}
inline bool isGroup(const QModelIndex &index) {
    return index.isValid() && index.internalPointer() == nullptr;
}
inline bool isConversation(const QModelIndex &index) {
    return index.isValid() && index.internalPointer() != nullptr;
}

}

ConversationsModel::ConversationsModel(TeamController &controller, Environment &environment)
    : mController(controller)
    , mEnvironment(environment)
{
}

int ConversationsModel::rowCount(const QModelIndex &parent) const
{
    if (isRoot(parent)) {
        return mGroups.size();
    }

    if (isGroup(parent)) {
        return mGroups.at(parent.row())->conversations.size();
    }

    return 0;
}

int ConversationsModel::columnCount(const QModelIndex &parent) const
{
    if (isRoot(parent)) {
        return 1;
    }

    if (isGroup(parent)) {
        return 2;
    }

    return 0;
}

QModelIndex ConversationsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (isRoot(parent)) {
        if (row < 0 || row >= mGroups.size() || column < 0 || column >= columnCount(parent)) {
            return {};
        }
        return createIndex(row, column, nullptr);
    }

    if (isGroup(parent)) {
        const auto &parentGroup = mGroups.at(parent.row());
        if (row < 0 || row >= parentGroup->conversations.size() || column < 0 || column >= columnCount(parent)) {
            return {};
        }
        return createIndex(row, column, mGroups.at(parent.row()).get());
    }

    return {};
}

QModelIndex ConversationsModel::parent(const QModelIndex &index) const
{
    if (isRoot(index)) {
        return {};
    }

    if (isGroup(index)) {
        return {};
    }

    auto group = static_cast<Group *>(index.internalPointer());
    const auto row = std::distance(mGroups.cbegin(), std::find_if(mGroups.cbegin(), mGroups.cend(),
                                                                  [group](const auto &g) { return g.get() == group; }));
    return createIndex(row, 0, nullptr);
}

QVariant ConversationsModel::data(const QModelIndex &index, int role) const
{
    if (isRoot(index)) {
        return {};
    }

    if (isGroup(index)) {
        const auto &group = mGroups.at(index.row());
        switch (role) {
        case Qt::DisplayRole:
            return group->name;
        default:
            return {};
        }
    }

    if (isConversation(index)) {
        const auto group = static_cast<Group *>(index.internalPointer());
        const auto &conversation = group->conversations.at(index.row());

        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
            case 0:
                return conversation->name();
            case 1:
                return conversation->unreadCountDisplay();
            }
            return {};
        default:
            return {};
        }
    }

    return {};
}

