#pragma once

#include <QObject>

#include <memory>

namespace Shirk::Core {

class Team;
class Environment;

class TeamController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

public:
    enum class Status {
        Connecting,
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

    Status status() const;

    void updateConfig();

Q_SIGNALS:
    void statusChanged(Status status);


protected:
    void setStatus(Status status);

private:
    Environment &mEnv;
    std::unique_ptr<Team> mTeam;

    Status mStatus = Status::Disconnected;
};

}
