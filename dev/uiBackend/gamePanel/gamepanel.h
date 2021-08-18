#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QObject>
#include <QTimer>
#include "../../baseTypes/baseTypes.h"

class GamePanel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentApm MEMBER m_currentApm NOTIFY currentApmUpdate)
    Q_PROPERTY(QString averageApm MEMBER m_averageApm NOTIFY averageApmUpdate)

    Q_PROPERTY(bool gamePanelVisible MEMBER m_gamePanelVisisble NOTIFY gamePanelVisibleChanged)
    Q_PROPERTY(bool racePanelVisible MEMBER m_racePanelVisisble NOTIFY racePanelVisibleChanged)

    Q_PROPERTY(QString player0Race MEMBER m_player0Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player1Race MEMBER m_player1Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player2Race MEMBER m_player2Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player3Race MEMBER m_player3Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player4Race MEMBER m_player4Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player5Race MEMBER m_player5Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player6Race MEMBER m_player6Race NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player7Race MEMBER m_player7Race NOTIFY playerTestStatsUpdate)

    Q_PROPERTY(QString player0Color MEMBER m_player0Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player1Color MEMBER m_player1Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player2Color MEMBER m_player2Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player3Color MEMBER m_player3Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player4Color MEMBER m_player4Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player5Color MEMBER m_player5Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player6Color MEMBER m_player6Color NOTIFY playerTestStatsUpdate)
    Q_PROPERTY(QString player7Color MEMBER m_player7Color NOTIFY playerTestStatsUpdate)

public:
    explicit GamePanel(QObject *parent = nullptr);

    void setGamePanelVisisble(bool newGamePanelVisisble);

signals:
    void currentApmUpdate();
    void averageApmUpdate();

    void gamePanelVisibleChanged(bool);
    void racePanelVisibleChanged(bool);
    void playerTestStatsUpdate();

public slots:
    void onCurrentApmChanged(quint64 val);
    void onAverageApmChanged(quint64 val);
    void onGameStopped();
    void receivePlayersTestStats(QVector<PlayerStats> testStats);

private slots:
    void racePanelVisibleTimerTimeout();

private:
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(int team);

private:
    QTimer* m_racePanelVisibleTimer;

    bool m_gamePanelVisisble = false;
    bool m_racePanelVisisble = false;

    QString m_currentApm;
    QString m_averageApm;

    QString m_player0Race = "";
    QString m_player1Race = "";
    QString m_player2Race = "";
    QString m_player3Race = "";
    QString m_player4Race = "";
    QString m_player5Race = "";
    QString m_player6Race = "";
    QString m_player7Race = "";

    QString m_player0Color = "";
    QString m_player1Color = "";
    QString m_player2Color = "";
    QString m_player3Color = "";
    QString m_player4Color = "";
    QString m_player5Color = "";
    QString m_player6Color = "";
    QString m_player7Color = "";
};

#endif // GAMEPANEL_H
