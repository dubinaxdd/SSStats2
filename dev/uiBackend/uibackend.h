#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>
#include "../baseTypes/baseTypes.h"
#include "QTimer"

class UiBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool expand MEMBER m_expand NOTIFY sendExpand)
    Q_PROPERTY(bool showClient MEMBER m_showClient NOTIFY sendShowClient)
    Q_PROPERTY(int mousePositionX READ mousePositionX)
    Q_PROPERTY(int mousePositionY READ mousePositionY)
    Q_PROPERTY(int mouseAreaWidth MEMBER m_mouseAreaWidth)
    Q_PROPERTY(int mouseAreaHeight MEMBER m_mouseAreaHeight)
    Q_PROPERTY(bool topmost READ getWindowTopmost WRITE setWindowTopmost)
    Q_PROPERTY(bool ssWindowed MEMBER m_ssWindowed NOTIFY ssWindowedModeChanged)
    Q_PROPERTY(int ssWindowPositionX MEMBER m_ssWindowPositionX NOTIFY ssWindowPositionChanged)
    Q_PROPERTY(int ssWindowPositionY MEMBER m_ssWindowPositionY NOTIFY ssWindowPositionChanged)


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
    explicit UiBackend(QObject *parent = nullptr);

    bool switchNoFogState() const;

    bool expand() const;

    bool getShowClient();

    void setNoFogState(bool state);

    void setExpand(bool newExpand);

    void mousePressEvent (QPoint mousePosition);
    void mouseMoveEvent (QPoint mousePosition);
    int mousePositionX();
    int mousePositionY();
    void setWindowTopmost(bool topmost);
    bool getWindowTopmost();
    void setMouseArea(int width, int height);

    void setSsWindowed(bool newSsWindowed);
    void setSsWindowPosition(int x, int y);

signals:
    void sendSwitchNoFogHoverState(bool);
    void switchNoFogStateChanged(bool);
    void noFogStateChanged(bool);

    void sendExpand(bool);

    void sendMousePress();
    void sendMouseMove();

    void sendShowClient(bool);
    void windowTopmostChanged();
    void windowedModeSeted();
    void gamePanelVisibleChanged(bool);
    void racePanelVisibleChanged(bool);
    void headerPanelVisibleChanged(bool);
    void playerTestStatsUpdate();
    void ssWindowedModeChanged();
    void ssWindowPositionChanged();

public slots:

    void onNoFogStateChanged(bool state);
    void onSwitchNoFogStateChanged(bool state);

    void expandKeyPressed();

    void receiveSsMaximized(bool maximized);
    void onSsLaunchStateChanged(bool state);
    void receivePlayersTestStats(QVector<PlayerStats> testStats);

    void onGameStarted();
    void onGameStopped();
    void onStartingMission();

private slots:
    void racePanelVisibleTimerTimeot();

private:
    void showClient();
    void replaceRaceKeyword(QString *raceString);
    QString chooseColorForPlayer(int team);

private:
    QTimer* racePanelVisibleTimer;

    bool m_expand = false;

    QPoint m_mousePosition;
    bool m_ssMaximized = false;
    bool m_ssLaunchState = false;
    bool m_showClient = false;

    bool m_windowTopmost = false;
    bool m_ssWindowed = false;

    int m_mouseAreaWidth;
    int m_mouseAreaHeight;

    int m_ssWindowPositionX = 0;
    int m_ssWindowPositionY = 0;

    bool m_gameStarted = false;
    bool m_missionStarted = false;

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

#endif // UIBACKEND_H
