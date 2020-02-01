#pragma once

#include "usermanager.h"
#include "conversationsmodel.h"
#include "utils/memory.h"

#include <QObject>

#include <memory>

namespace Shirk::Core {

class Team;
class Environment;
class ConversationsModel;
class RTMController;

class TeamController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    enum class Status {
        Connecting,
        Reconnecting,
        Connected,
        Disconnected
    };
    Q_ENUM(Status)

    TeamController(Environment &environment, std::unique_ptr<Team> team);
    ~TeamController() override;

    /**
     * Starts to connect to Slack.
     */
    void start();
    /**
     * Disconnects from Slack.
     */
    void quit();

    const Team &team() const;
    Team &team();

    UserManager &userManager() {
        return mUserManager;
    }
    ConversationsModel &conversations() {
        return mConversations;
    }

    Status status() const;

    void updateConfig();

Q_SIGNALS:
    void statusChanged(Status status);


protected:
    void setStatus(Status status);

private:
    Environment &mEnv;
    std::unique_ptr<Team> mTeam;

    UserManager mUserManager;
    ConversationsModel mConversations;
    UniqueQObjectPtr<RTMController> mRTMController;

    Status mStatus = Status::Disconnected;
};

}
