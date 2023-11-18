#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>
#include <replaysListModel.h>
#include <playersListModel.h>
#include <QUrl>
#include <imageProvider.h>
#include <asyncReplayReader.h>
#include <QThread>

class ReplayManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ReplaysListModel* replaysListModel MEMBER m_replaysListModel NOTIFY replaysListModelSetded)
    Q_PROPERTY(PlayersListModel* playersListModel MEMBER m_playersListModel NOTIFY replaysListModelSetded)

    Q_PROPERTY(QUrl currentFilePath MEMBER m_currentFilePath NOTIFY updateReplayInfo)
    Q_PROPERTY(QUrl ssUrlPathPath MEMBER m_ssUrlPathPath NOTIFY updateReplayInfo)
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
    ~ReplayManager();

    void setSsPath(const QString &newSsPath);

    Q_INVOKABLE void openPlaybackFolder();

    Q_INVOKABLE void update();
    Q_INVOKABLE void removeReplay(QString fileName);
    Q_INVOKABLE void saveReplay(QString fileName);

    Q_INVOKABLE void saveBadges(QString filePath, QString imageUrl, int width, int height);

    Q_INVOKABLE void choiseOtherPlaybackFolder(QString folder);
    Q_INVOKABLE void choiseDefaultPlaybackFolder();

    Q_INVOKABLE void findReplays(QString findText);

    const QString &winConditions() const;
    void setWinConditions(const QString &newWinConditions);

    const QString &gameSettings() const;
    void setGameSettings(const QString &newGameSettings);

public slots:
    void openPlayback(QString fileName);
    void receiveModsInfo(QList <ModInfo> modInfo);

private slots:
    void receiveReplaysInfo(QVector<ReplayListInfo> newReplaysList);

signals:
    void replaysListModelSetded();
    void updateReplayInfo();
    void winConditionsChanged();
    void gameSettingsChanged();

    void requestReplaysInfo(QString playbackFolder, QString findText);

private:
    void getReplaysData();
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(uint team);

    void resetView();

private:
    QString m_playbackFolder;
    QString m_ssPath;
    ReplaysListModel* m_replaysListModel;
    PlayersListModel* m_playersListModel;

    ImageProvider* p_imageProvider;
    AsyncReplayReader* m_asyncReplayReader;
    QThread m_asyncReplayReaderThread;

    QUrl m_currentFilePath;
    QUrl m_ssUrlPathPath;
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

    QList <ModInfo> m_modInfo;
};

#endif // REPLAYMANAGER_H
