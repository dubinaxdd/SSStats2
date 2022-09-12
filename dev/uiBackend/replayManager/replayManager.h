#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>
#include <replaysListModel.h>
#include <playersListModel.h>
#include <QUrl>
#include <imageProvider.h>

class ReplayManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ReplaysListModel* replaysListModel MEMBER m_replaysListModel NOTIFY replaysListModelSetded)
    Q_PROPERTY(PlayersListModel* playersListModel MEMBER m_playersListModel NOTIFY replaysListModelSetded)

    Q_PROPERTY(QUrl currentFilePath MEMBER m_currentFilePath NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentReplayName MEMBER m_currentReplayName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentFileName MEMBER m_currentFileName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMod MEMBER m_currentMod NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentModVersion MEMBER m_currentModVersion NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMap MEMBER m_currentMap NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentDuration MEMBER m_currentDuration NOTIFY updateReplayInfo)
    Q_PROPERTY(QString mapSourceUrl MEMBER m_mapSourceUrl NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentReplayTime MEMBER m_currentReplayTime NOTIFY updateReplayInfo)

    Q_PROPERTY(QString winConditions READ winConditions WRITE setWinConditions NOTIFY winConditionsChanged)
    Q_PROPERTY(QString gameSettings READ gameSettings WRITE setGameSettings NOTIFY gameSettingsChanged)

    Q_PROPERTY(int currentPlayerCount MEMBER m_currentPlayerCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentTeamsCount MEMBER m_currentTeamsCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentMapSize MEMBER m_currentMapSize NOTIFY updateReplayInfo)

public:
    explicit ReplayManager(ImageProvider* imageProvider, QObject *parent = nullptr);

    void setSsPath(const QString &newSsPath);

    Q_INVOKABLE void openPlaybackFolder();

    Q_INVOKABLE void update();
    Q_INVOKABLE void removeReplay(QString fileName);
    Q_INVOKABLE void saveReplay(QString fileName);

    const QString &winConditions() const;
    void setWinConditions(const QString &newWinConditions);

    const QString &gameSettings() const;
    void setGameSettings(const QString &newGameSettings);

public slots:
    void openPlayback(QString fileName);

signals:
    void replaysListModelSetded();
    void updateReplayInfo();
    void winConditionsChanged();
    void gameSettingsChanged();

private:
    void getReplaysData();
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(uint team);

private:
    QString m_ssPath;
    ReplaysListModel* m_replaysListModel;
    PlayersListModel* m_playersListModel;

    ImageProvider* p_imageProvider;

    QUrl m_currentFilePath;
    QString m_currentReplayName;
    QString m_currentFileName;
    QString m_currentMod;
    QString m_currentModVersion;
    QString m_currentMap;
    QString m_currentDuration;
    QString m_currentReplayTime;

    QString m_winConditions;
    QString m_gameSettings;

    int m_currentPlayerCount;
    int m_currentTeamsCount;
    int m_currentMapSize;

    QString m_mapSourceUrl;

};

#endif // REPLAYMANAGER_H
