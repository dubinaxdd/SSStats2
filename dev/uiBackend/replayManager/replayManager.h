#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>
#include <replaysListModel.h>

class ReplayManager : public QObject
{
    Q_OBJECT
public:
    explicit ReplayManager(QObject *parent = nullptr);

    Q_PROPERTY(ReplaysListModel* replaysListModel MEMBER m_replaysListModel NOTIFY replaysListModelSetded)

    void setSsPath(const QString &newSsPath);

    Q_INVOKABLE void openPlaybackFolder();

signals:
    void replaysListModelSetded();

private:
    void getReplaysDirs();

private:
    QString m_ssPath;
    ReplaysListModel* m_replaysListModel;


};

#endif // REPLAYMANAGER_H
