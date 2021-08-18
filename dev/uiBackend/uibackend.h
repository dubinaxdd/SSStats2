#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>
#include "QTimer"

#include "gamePanel/gamepanel.h"
#include "statisticPanel/statisticpanel.h"
#include "imageProvider/imageprovider.h"

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

    Q_PROPERTY(GamePanel* gamePanel MEMBER m_gamePanel NOTIFY gamePanelInitialized)
    Q_PROPERTY(StatisticPanel* statisticPanel MEMBER m_statisticPanel NOTIFY statisticPanelInitialized)


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


    GamePanel *gamePanel() const;
    StatisticPanel *statisticPanel() const;

    ImageProvider *imageProvider() const;

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

    void headerPanelVisibleChanged(bool);

    void ssWindowedModeChanged();
    void ssWindowPositionChanged();
    void gamePanelInitialized();
    void statisticPanelInitialized();

public slots:

    void onNoFogStateChanged(bool state);
    void onSwitchNoFogStateChanged(bool state);

    void expandKeyPressed();

    void receiveSsMaximized(bool maximized);
    void onSsLaunchStateChanged(bool state);


    void onGameStarted();
    void onGameStopped();
    void onStartingMission();

private:
    void showClient();

private:
    ImageProvider* m_imageProvider;
    GamePanel* m_gamePanel;
    StatisticPanel* m_statisticPanel;

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


};

#endif // UIBACKEND_H
