#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>
#include <replaysListModel.h>
#include <playersListModel.h>

class ReplayManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ReplaysListModel* replaysListModel MEMBER m_replaysListModel NOTIFY replaysListModelSetded)
    Q_PROPERTY(PlayersListModel* playersListModel MEMBER m_playersListModel NOTIFY replaysListModelSetded)

    Q_PROPERTY(QString currentFilePath MEMBER m_currentFilePath NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentReplayName MEMBER m_currentReplayName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentFileName MEMBER m_currentFileName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMod MEMBER m_currentMod NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentModVersion MEMBER m_currentModVersion NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMap MEMBER m_currentMap NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentDuration MEMBER m_currentDuration NOTIFY updateReplayInfo)
    Q_PROPERTY(QString mapSourceUrl MEMBER m_mapSourceUrl NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentReplayTime MEMBER m_currentReplayTime NOTIFY updateReplayInfo)

    Q_PROPERTY(int currentPlayerCount MEMBER m_currentPlayerCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentTeamsCount MEMBER m_currentTeamsCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentMapSize MEMBER m_currentMapSize NOTIFY updateReplayInfo)

public:
    explicit ReplayManager(QObject *parent = nullptr);

    void setSsPath(const QString &newSsPath);

    Q_INVOKABLE void openPlaybackFolder();
    //Q_INVOKABLE void openPlayback(QString fileName);

    Q_INVOKABLE void update();
    Q_INVOKABLE void removeReplay(QString fileName);

public slots:
    void openPlayback(QString fileName);

signals:
    void replaysListModelSetded();
    void updateReplayInfo();

private:
    void getReplaysData();
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(uint team);

private:
    QString m_ssPath;
    ReplaysListModel* m_replaysListModel;
    PlayersListModel* m_playersListModel;

    QString m_currentFilePath;
    QString m_currentReplayName;
    QString m_currentFileName;
    QString m_currentMod;
    QString m_currentModVersion;
    QString m_currentMap;
    QString m_currentDuration;
    QString m_currentReplayTime;

    int m_currentPlayerCount;
    int m_currentTeamsCount;
    int m_currentMapSize;

    QString m_mapSourceUrl;
};

#endif // REPLAYMANAGER_H
