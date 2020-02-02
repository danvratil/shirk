#include "conversationsmodel.h"
#include "teamcontroller.h"
#include "environment.h"
#include "networkdispatcher.h"
#include "slackapi/conversations.h"
#include "utils/memory.h"
#include "utils/compiler.h"

#include <memory>
#include <qnamespace.h>

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

Future<void> ConversationsModel::populate()
{
    Promise<void> promise;
    auto future = promise.getFuture();
// FIXME: I suspect this is just GCC being overly-sensitive
compiler_suppress_warning("-Wmissing-field-initializers")
    SlackAPI::ConversationsListRequest request{
        .types = {SlackAPI::ConversationsListRequest::Type::PublicChannel,
                  SlackAPI::ConversationsListRequest::Type::PrivateChannel,
                  SlackAPI::ConversationsListRequest::Type::MPIM,
                  SlackAPI::ConversationsListRequest::Type::IM}
    };
compiler_restore_warning("-Wmissing-field-initializers")
    mEnvironment.networkDispatcher.sendRequest(mController.team(), std::move(request))
        .then([this, promise = std::move(promise)](const QJsonValue &value) mutable {
            const auto resp = SlackAPI::ConversationsListResponse::parse(value);
            beginResetModel();
            mGroups.clear();
            mGroups.emplace_back(std::make_unique<Group>(tr("Channels")));
            mGroups.emplace_back(std::make_unique<Group>(tr("Direct")));
            for (const auto &chann: resp.channels) {
                auto conv = std::make_unique<Conversation>(chann.id, mController.userManager());
                conv->updateFromConversation(chann);
                connect(conv.get(), &Conversation::conversationChanged,
                        this, [this, conv = conv.get()]() {
                            const auto index = indexForConversation(conv);
                            Q_EMIT dataChanged(index, index);
                        });

                groupForConversation(conv.get())->conversations.push_back(std::move(conv));
            }
            endResetModel();
            promise.setResult();
        });

    return future;
}

QModelIndex ConversationsModel::indexForGroup(const Group *group) const
{
    const auto pos = std::distance(mGroups.cbegin(), std::find_if(mGroups.cbegin(), mGroups.cend(), UniquePtrCompare{group}));
    return index(pos, 0, {});
}

QModelIndex ConversationsModel::indexForConversation(const Conversation *conv) const
{
    const auto group = groupForConversation(conv);
    const auto pos = std::distance(group->conversations.cbegin(), std::find(group->conversations.cbegin(),
                                    group->conversations.cend(), UniquePtrCompare{conv}));
    return index(pos, 0, indexForGroup(group));
}

ConversationsModel::Group *ConversationsModel::groupForConversation(const Conversation *conv) const
{
    switch (conv->type()) {
    case Conversation::Type::Channel:
    case Conversation::Type::Group:
        return mGroups.at(0).get();
    case Conversation::Type::IM:
    case Conversation::Type::MPIM:
        return mGroups.at(1).get();
    }

    return mGroups.at(0).get();
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
        if (row < 0 || row >= static_cast<int>(mGroups.size())
                || column < 0 || column >= columnCount(parent)) {
            return {};
        }
        return createIndex(row, column, nullptr);
    }

    if (isGroup(parent)) {
        const auto &parentGroup = mGroups.at(parent.row());
        if (row < 0 || row >= static_cast<int>(parentGroup->conversations.size())
                || column < 0 || column >= columnCount(parent)) {
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
                switch (conversation->type()) {
                case Conversation::Type::Channel:
                case Conversation::Type::Group:
                    return conversation->name();
                case Conversation::Type::IM:
                case Conversation::Type::MPIM:
                    return conversation->user()->name();
                }
                return {};
            case 1:
                return conversation->unreadCountDisplay();
            }
            return {};
        case Qt::ForegroundRole:
            if (conversation->isMember()) {
                return QColor{Qt::black};
            } else {
                return QColor{Qt::gray};
            }
        default:
            return {};
        }
    }

    return {};
}

