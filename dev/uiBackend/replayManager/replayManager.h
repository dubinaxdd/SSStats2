#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>
#include <replaysListModel.h>

class ReplayManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentReplayName MEMBER m_currentReplayName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentFileName MEMBER m_currentFileName NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMod MEMBER m_currentMod NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentModVersion MEMBER m_currentModVersion NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentMap MEMBER m_currentMap NOTIFY updateReplayInfo)
    Q_PROPERTY(QString currentDuration MEMBER m_currentDuration NOTIFY updateReplayInfo)

    Q_PROPERTY(int currentPlayerCount MEMBER m_currentPlayerCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentTeamsCount MEMBER m_currentTeamsCount NOTIFY updateReplayInfo)
    Q_PROPERTY(int currentMapSize MEMBER m_currentMapSize NOTIFY updateReplayInfo)

public:
    explicit ReplayManager(QObject *parent = nullptr);

    Q_PROPERTY(ReplaysListModel* replaysListModel MEMBER m_replaysListModel NOTIFY replaysListModelSetded)

    void setSsPath(const QString &newSsPath);

    Q_INVOKABLE void openPlaybackFolder();
    Q_INVOKABLE void openPlayback(QString fileName);

signals:
    void replaysListModelSetded();
    void updateReplayInfo();

private:
    void getReplaysDirs();

private:
    QString m_ssPath;
    ReplaysListModel* m_replaysListModel;

    QString m_currentReplayName;
    QString m_currentFileName;
    QString m_currentMod;
    QString m_currentModVersion;
    QString m_currentMap;
    QString m_currentDuration;

    int m_currentPlayerCount;
    int m_currentTeamsCount;
    int m_currentMapSize;
};

#endif // REPLAYMANAGER_H
