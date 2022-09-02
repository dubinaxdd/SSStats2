#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <QObject>

class ReplayManager : public QObject
{
    Q_OBJECT
public:
    explicit ReplayManager(QObject *parent = nullptr);

    void setSsPath(const QString &newSsPath);

private:
    void getReplaysDirs();

private:
    QString m_ssPath;

};

#endif // REPLAYMANAGER_H
