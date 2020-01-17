#pragma once

#include <QObject>
#include <QUrl>
#include <QIcon>

#include <memory>
#include <optional>

class QJsonValue;
class QSettings;

namespace Shirk::SlackAPI {
class TeamInfoResponse;
}

namespace Shirk::Core
{

class Team : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY teamChanged)
    Q_PROPERTY(QString domain READ domain NOTIFY teamChanged)
    Q_PROPERTY(QIcon icon READ icon NOTIFY teamChanged)
    Q_PROPERTY(QString accessToken READ accessToken NOTIFY teamChanged)

public:
    Team(const QString &id, const QString &name, const QString &accessToken);
    ~Team() = default;

    QString id() const;
    QString name() const;
    QString domain() const;
    QString accessToken() const;
    QIcon icon() const;

    static std::unique_ptr<Team> fromSettings(QSettings *settings);
    void updateFromTeamInfo(const SlackAPI::TeamInfoResponse &team);
    void updateConfig(QSettings *settings);

Q_SIGNALS:
    void teamChanged();

protected:
    Team() = default;

private:
    QString mId;
    QString mName;
    QString mDomain;
    QString mAccessToken;
    QUrl mIconUrl;
    std::optional<QIcon> mIcon;
};

}
