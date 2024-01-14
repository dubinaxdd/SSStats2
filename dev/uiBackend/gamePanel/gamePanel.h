#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QObject>
#include <QTimer>
#include <baseTypes.h>
#include <settingscontroller.h>

class SettingsController;

class GamePanel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentApm MEMBER m_currentApm NOTIFY apmUpdate)
    Q_PROPERTY(QString averageApm MEMBER m_averageApm NOTIFY apmUpdate)

    Q_PROPERTY(bool gamePanelVisible MEMBER m_gamePanelVisible NOTIFY gamePanelVisibleChanged)
    Q_PROPERTY(bool racePanelVisible MEMBER m_racePanelVisible NOTIFY racePanelVisibleChanged)
    Q_PROPERTY(bool gameLeaveTimerVisible MEMBER m_gameLeaveTimerVisible NOTIFY gameLeaveTimerVisibleChanged)
    Q_PROPERTY(int gameLeaveTimeLeft MEMBER m_gameLeaveTimeLeft NOTIFY gemeLeaveTimeLeftChanged)

    Q_PROPERTY(bool smallPannelActive READ getSmallGamePanelActive WRITE setSmallGamePanelActive NOTIFY smallPannelActiveChanged)
    Q_PROPERTY(bool showGamePannelPreset READ showGamePannelPreset WRITE setShowGamePannelPreset NOTIFY showGamePanelPresetChanged)

    Q_PROPERTY(bool gameRankedMode READ gameRankedMode WRITE setGameRankedMode NOTIFY gameRankedModeChanged)
    Q_PROPERTY(bool rankedModePanelVisible READ rankedModePanelVisible WRITE setRankedModePanelVisible NOTIFY rankedModePanelVisibleChanged)

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
    explicit GamePanel(SettingsController* settingsController, QObject *parent = nullptr);

    void setGamePanelVisible(bool newGamePanelVisible);

    void setSmallGamePanelActive(bool active);
    bool getSmallGamePanelActive();

    bool showGamePannelPreset() const;
    void setShowGamePannelPreset(bool showGamePannelPreset);

    const QString &currentApm() const;

    const QString &averageApm() const;

    bool gameRankedMode() const;
    void setGameRankedMode(bool newGameRankedMode);

    bool rankedModePanelVisible() const;
    void setRankedModePanelVisible(bool newRankedModePanelVisible);

signals:
    void apmUpdate();

    void gamePanelVisibleChanged(bool);
    void racePanelVisibleChanged(bool);
    void playerTestStatsUpdate();

    void gameLeaveTimerVisibleChanged(bool);
    void gemeLeaveTimeLeftChanged(int);
    void smallPannelActiveChanged(bool);
    void showGamePanelPresetChanged(bool);

    void gameRankedModeChanged();

    void rankedModePanelVisibleChanged();

public slots:
    void onApmChanged(int capm, int aapm);
    void onGameStopped();
    void onGameStarted(SsMissionState gameCurrentState);
    void receivePlayersTestStats(QVector<PlayerStats> testStats);
    void expandPlayerRacesButtonClick();
    void onSettingsLoaded();


private slots:
    void racePanelVisibleTimerTimeout();
    void gameLeaveTimerTimeout();

private:
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(int team);
    void updatePlayerRaces();

private:
    SettingsController* m_settingsController;

    QTimer* m_racePanelVisibleTimer;
    QTimer* m_gameLeaveTimer;

    QVector<PlayerStats> m_testStats;

    int m_gameLeaveTimeLeft = 0;
    bool m_gameLeaveTimerVisible = false;

    bool m_smallPannelActive;
    bool m_showGamePannelPreset;


    bool m_gamePanelVisible = false;
    bool m_racePanelVisible = false;

    bool m_gameRankedMode = true;
    bool m_rankedModePanelVisible = true;

    QString m_currentApm = "-";
    QString m_averageApm = "-";

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
