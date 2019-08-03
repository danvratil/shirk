#pragma once

#include <QObject>

#include <memory>

namespace Core {

class Team;
class Config;

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

    TeamController(Config &config, std::unique_ptr<Team> team);
    ~TeamController() override;

    /**
     * Starts to connect to Slack.
     */
    void start();
    /**
     * Disconnects from Slack.
     */
    void quit();


    Team *team() const;

    Status status() const;


Q_SIGNALS:
    void statusChanged(Status status);


protected:
    void setStatus(Status status);

private:
    Config &mConfig;
    std::unique_ptr<Team> mTeam;

    Status mStatus = Status::Disconnected;
};

}
