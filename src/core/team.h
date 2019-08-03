#pragma once

#include <QObject>
#include <QUrl>
#include <QIcon>

#include <memory>
#include <optional>

class QJsonValue;
class QSettings;

namespace Core
{

namespace API {
class TeamInfoResponse;
}

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

    std::unique_ptr<Team> fromSettings(const QSettings &settings);
    void updateFromTeamInfo(const API::TeamInfoResponse &team);

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
