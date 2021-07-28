#ifndef GAMEINFOREADER_H
#define GAMEINFOREADER_H

#include <QObject>
#include <QTimer>

class GameInfoReader : public QObject
{
    Q_OBJECT
public:
    explicit GameInfoReader(QString sspath, QObject *parent = nullptr);

private slots:
    void readGameInfo();
    void readGameParametresAfterStop();

signals:
    void gameStoped();
    void gameStarted();

private:
    QTimer* m_gameInfoReadTimer;
    QString m_ssPath;

    bool m_gameStoped = true;
    bool m_gameStarted = false;
    bool m_gamePlayback = false;
    bool m_gameLoad = false;
    bool m_scrimishMission = false;

};

#endif // GAMEINFOREADER_H
