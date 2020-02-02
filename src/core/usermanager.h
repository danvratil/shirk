#pragma once

#include "user.h"
#include "utils/compat.h"

#include <optional>
#include <functional>

namespace Shirk::Core
{

class Team;
struct Environment;

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(const Team &team, Environment &environment);
    ~UserManager() override = default;

    const Team &team() const {
        return mTeam;
    }

    std::optional<std::reference_wrapper<const UserData>> getUserData(const User::Id &id);

    using DataCallback = std::function<void(const User::Id &, const UserData &userData)>;
    void requestData(User *user, DataCallback &&cb);
    void cancelPendingRequest(User *);

    struct PendingRequest {
        User::Id userId;
        std::vector<std::pair<User*, DataCallback>> requests;
    };
private:
    std::vector<PendingRequest> mPendingRequests;
    std::unordered_map<User::Id, UserData> mUserData;
    const Team &mTeam;
    const Environment &mEnvironment;
};

}
