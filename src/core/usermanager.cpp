#include "usermanager.h"
#include "environment.h"
#include "networkdispatcher.h"
#include "iconloader.h"
#include "slackapi/user.h"
#include <functional>

using namespace Shirk::Core;

namespace {

struct FindUserById {
    FindUserById(const User::Id &id)
        : mId(id) {}

    bool operator()(const UserManager::PendingRequest &request) const
    {
        return request.userId == mId;
    }

private:
    const User::Id &mId;
};

}

UserManager::UserManager(const Team &team, Environment &environment)
    : mTeam(team)
    , mEnvironment(environment)
{}

std::optional<std::reference_wrapper<const UserData>> UserManager::getUserData(const User::Id &id)
{
    auto it = mUserData.find(id);
    if (it == mUserData.cend()) {
        return std::nullopt;
    }

    return std::cref(it->second);
}

void UserManager::requestData(User *user, DataCallback &&cb)
{
    if (const auto dataIt = mUserData.find(user->id()); dataIt != mUserData.end()) {
        QMetaObject::invokeMethod(user, [this, cb = std::move(cb), user]() {
            auto data = mUserData.find(user->id());
            Q_ASSERT(data != mUserData.cend());
            cb(user->id(), data->second);
        }, Qt::QueuedConnection);
        return;
    }


    if (const auto userIt = std::find_if(mPendingRequests.begin(), mPendingRequests.end(), FindUserById{user->id()});
            userIt != mPendingRequests.end()) {
        userIt->requests.emplace_back(user, std::move(cb));
    } else {
        mPendingRequests.emplace_back(PendingRequest{user->id(), {{user, std::move(cb)}}});

        SlackAPI::UserInfoRequest request{.id = user->id()};
        mEnvironment.networkDispatcher.sendRequest(mTeam, request, this, [this](const QJsonValue &result) {
            auto resp = SlackAPI::UserInfoResponse::parse(result);
            IconLoader::load({resp.info.profile.image_24, resp.info.profile.image_32, resp.info.profile.image_48,
                              resp.info.profile.image_72, resp.info.profile.image_192, resp.info.profile.image_512 },
              [this, resp = std::move(resp)](const QIcon &icon) {
                  const auto userId = resp.info.id;
                  auto userData = UserData::fromAPI(resp.info);
                  userData.avatar = icon;

                  auto dataIt = mUserData.insert(std::make_pair(userId, std::move(userData)));
                  auto userIt = std::find_if(mPendingRequests.begin(), mPendingRequests.end(), FindUserById{userId});
                  Q_ASSERT(userIt != mPendingRequests.end());

                  for (auto &cb: userIt->requests) {
                      cb.second(dataIt.first->first, dataIt.first->second);
                  }

                  mPendingRequests.erase(userIt);
              });
        });
    }
}

void UserManager::cancelPendingRequest(User *user)
{
    if (const auto userIt = std::find_if(mPendingRequests.begin(), mPendingRequests.end(), FindUserById{user->id()});
            userIt != mPendingRequests.end()) {
        auto &requests = userIt->requests;
        if (const auto requestIt = std::find_if(requests.begin(), requests.end(), [user](const auto &v) { return v.first == user; });
                requestIt != requests.end()) {
            requests.erase(requestIt);
        }
        if (requests.empty()) {
            mPendingRequests.erase(userIt);
        }
    }
}
