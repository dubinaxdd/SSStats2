#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>
#include <QTimer>

#include <gamepanel.h>
#include <statisticpanel.h>
#include <imageprovider.h>
#include <settingscontroller.h>
#include <MessagesPage.h>
#include <SettingsPageModel.h>

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

    Q_PROPERTY(bool headerVisible MEMBER m_headerVisible NOTIFY headerPanelVisibleChanged)
    Q_PROPERTY(bool patyStatisticVisible MEMBER m_patyStatisticVisible NOTIFY patyStatisticVisibleChanged)

    Q_PROPERTY(QString ssStatsVersion MEMBER m_ssStatsVersion NOTIFY statsHeaderInitialized)

    Q_PROPERTY(GamePanel* gamePanel MEMBER m_gamePanel NOTIFY gamePanelInitialized)
    Q_PROPERTY(StatisticPanel* statisticPanel MEMBER m_statisticPanel NOTIFY statisticPanelInitialized)

    Q_PROPERTY(bool noFogState READ getFogState WRITE setNoFogState NOTIFY noFogStateChanged)
    Q_PROPERTY(float sizeModifer MEMBER m_sizeModifer NOTIFY sizeModiferChanged)

    Q_PROPERTY(MessagesPage* newsPage MEMBER m_newsPage NOTIFY statisticPanelInitialized)
    Q_PROPERTY(MessagesPage* eventsPage MEMBER m_eventsPage NOTIFY statisticPanelInitialized)
    Q_PROPERTY(SettingsPageModel* settingsPageModel MEMBER m_settingsPageModel NOTIFY statisticPanelInitialized)

    Q_PROPERTY(QString lastNotification MEMBER m_lastNotification NOTIFY updateNotification)
    Q_PROPERTY(bool lastNotificationIsWarning MEMBER m_lastNotificationIsWarning NOTIFY updateNotification)
    Q_PROPERTY(bool notificationVisible READ notificationVisible WRITE setNotificationVisible NOTIFY notificationVisibleChanged)

public:
    explicit UiBackend(SettingsController* settingsController, QObject *parent = nullptr);

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
    bool getFogState() const;
    void onMouseWheel(int delta);

    GamePanel *gamePanel() const;
    StatisticPanel *statisticPanel() const;
    ImageProvider *imageProvider() const;
    MessagesPage *newsPage() const;
    MessagesPage *eventsPage() const;

    Q_INVOKABLE void launchSoulstorm();

    SettingsPageModel *settingsPageModel() const;

    bool notificationVisible() const;
    void setNotificationVisible(bool newNotificationVisible);

signals:
    void sendSwitchNoFogHoverState(bool);

    void sendExpand(bool);

    void sendMousePress();
    void sendMouseMove();
    void sendMouseWheel(int delta);

    void sendShowClient(bool);
    void windowTopmostChanged();
    void windowedModeSeted();

    void headerPanelVisibleChanged();
    void patyStatisticVisibleChanged();

    void ssWindowedModeChanged();
    void ssWindowPositionChanged();

    void gamePanelInitialized();
    void statisticPanelInitialized();
    void statsHeaderInitialized();
    void sendExit();
    void sendLaunchSoulstormWithSupportMode();
    void sendLaunchSoulstorm();
    void noFogStateChanged(bool);

    void sizeModiferChanged(float sizeModifer);
    void sizeModiferLoadedFromSettings(float scale);

    void expandButtonPressed();
    void updateNotification();
    void notificationVisibleChanged();

public slots:
    void expandKeyPressed();
    void expandPatyStatisticButtonClick();

    void receiveSsMaximized(bool maximized);
    void onSsLaunchStateChanged(bool state);

    void setMissionCurrentState(SsMissionState gameCurrentState);

    void receiveNotification(QString notify, bool isWarning);

    Q_INVOKABLE void onExit();
    Q_INVOKABLE void onLaunchSoulstormWithSupportMode();
    Q_INVOKABLE void setSizeModifer(float size);

private slots:
    void onSettingsLoaded();

private:
    void showClient();

    void loadStarted();
    void gameStopped();
    void startingMission(SsMissionState gameCurrentState);
    void gameOver();

private:
    ImageProvider* m_imageProvider;
    GamePanel* m_gamePanel;
    StatisticPanel* m_statisticPanel;
    SettingsController* m_settingsController;
    MessagesPage* m_newsPage;
    MessagesPage* m_eventsPage;
    SettingsPageModel* m_settingsPageModel;

    QTimer* m_notificationVisibleTimer;

    QString m_ssStatsVersion;

    SsMissionState m_gameCurrentState;

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

    bool m_loadStarted = false;
    bool m_missionStarted = false;

    bool m_headerVisible = true;
    bool m_patyStatisticVisible = true;

    bool m_patyStatisticVisibleButtonPressedState = false;

    bool m_noFogState = false;

    float m_sizeModifer = 2.0;

    QString m_lastNotification;
    bool m_lastNotificationIsWarning = false;
    bool m_notificationVisible = false;
};

#endif // UIBACKEND_H
