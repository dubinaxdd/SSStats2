#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <gamePanel.h>
#include <statisticPanel.h>
#include <imageProvider.h>
#include <settingsController.h>
#include <messagesPage.h>
#include <settingsPageModel.h>
#include <replayManager.h>
#include <mapManagerPage.h>
#include <modsPage.h>
#include <onlineStatisticPanel.h>
#include <balanceModPage.h>
#include <notificationManager.h>

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

    Q_PROPERTY(QString ssStatsVersion MEMBER m_ssStatsVersion CONSTANT )

    Q_PROPERTY(bool noFogState READ getFogState WRITE setNoFogState NOTIFY noFogStateChanged)
    Q_PROPERTY(double sizeModifer MEMBER m_sizeModifer NOTIFY sizeModiferChanged)

    Q_PROPERTY(GamePanel* gamePanel MEMBER m_gamePanel CONSTANT)
    Q_PROPERTY(StatisticPanel* statisticPanel MEMBER m_statisticPanel CONSTANT)
    Q_PROPERTY(MessagesPage* newsPage MEMBER m_newsPage  CONSTANT)
    Q_PROPERTY(MessagesPage* eventsPage MEMBER m_eventsPage  CONSTANT)
    Q_PROPERTY(SettingsPageModel* settingsPageModel MEMBER m_settingsPageModel  CONSTANT)
    Q_PROPERTY(ReplayManager* replayManager MEMBER m_replayManager  CONSTANT)
    Q_PROPERTY(MapManagerPage* mapManagerPage MEMBER m_mapManagerPage  CONSTANT)
    Q_PROPERTY(ModsPage* modsPage MEMBER m_modsPage  CONSTANT)
    Q_PROPERTY(OnlineStatisticPanel* onlineStatisticPanel MEMBER m_onlineStatisticPanel CONSTANT)
    Q_PROPERTY(BalanceModPage * balanceModPage MEMBER m_balanceModPage CONSTANT)
    Q_PROPERTY(NotificationManager * notificationManager MEMBER m_notificationManager CONSTANT)

    Q_PROPERTY(QString lastNotification MEMBER m_lastNotification NOTIFY updateNotification)
    Q_PROPERTY(bool lastNotificationIsWarning MEMBER m_lastNotificationIsWarning NOTIFY updateNotification)
    Q_PROPERTY(bool notificationVisible READ notificationVisible WRITE setNotificationVisible NOTIFY notificationVisibleChanged)

    Q_PROPERTY(bool rankedModeState READ rankedModeState WRITE setRankedModeState NOTIFY rankedModeStateChanged)
    Q_PROPERTY(bool enableTrainingModeSwitch READ enableTrainingModeSwitch WRITE setEnableTrainingModeSwitch NOTIFY enableTrainingModeSwitchChanged)
    Q_PROPERTY(int onlineCount READ onlineCount WRITE setOnlineCount NOTIFY onlineCountChanged)

    Q_PROPERTY(QString currentModName READ currentModName WRITE setCurrentModName NOTIFY currentModNameChanged)
    Q_PROPERTY(QString currentModTechnicalName READ currentModTechnicalName WRITE setCurrentModTechnicalName NOTIFY currentModTechnicalNameChanged)

    Q_PROPERTY(bool latesBalanceModNotInstalledDialogVisible READ latesBalanceModNotInstalledDialogVisible WRITE setLatesBalanceModNotInstalledDialogVisible NOTIFY latesBalanceModNotInstalledDialogVisibleChanged)
    Q_PROPERTY(bool ssNotInstalledDialogVisisble READ ssNotInstalledDialogVisisble WRITE setSsNotInstalledDialogVisisble NOTIFY ssNotInstalledDialogVisisbleChanged)
    Q_PROPERTY(bool steamNotInstalledDialogVisisble READ steamNotInstalledDialogVisisble WRITE setSteamNotInstalledDialogVisisble NOTIFY steamNotInstalledDialogVisisbleChanged)

    Q_PROPERTY(bool soulstormIsInstalled READ soulstormIsInstalled NOTIFY soulstormIsInstalledChanged)
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
    ReplayManager *replayManager() const;
    SettingsPageModel *settingsPageModel() const;

    bool soulstormIsInstalled();

    Q_INVOKABLE void launchSoulstorm();

    bool notificationVisible() const;
    void setNotificationVisible(bool newNotificationVisible);

    bool rankedModeState() const;
    void setRankedModeState(bool newRankedModeState);

    bool enableTrainingModeSwitch() const;
    void setEnableTrainingModeSwitch(bool newEnableTrainingModeSwitch);

    int onlineCount() const;
    void setOnlineCount(int newOnlineCount);

    void determinateRankedModePanelVisisble();

    MapManagerPage *mapManagerPage() const;

    const QString &currentModName() const;
    void setCurrentModName(const QString &newCurrentModName);

    const QString currentModTechnicalName() const;
    void setCurrentModTechnicalName(const QString &newCurrentModTechnicalName);

    ModsPage *modsPage() const;
    OnlineStatisticPanel *onlineStatisticPanel() const;
    BalanceModPage *balanceModPage() const;


    bool latesBalanceModNotInstalledDialogVisible() const;
    void setLatesBalanceModNotInstalledDialogVisible(bool newLatesBalanceModNotInstalledDialogVisible);

    void setSsPath(const QString &newSsPath);

    bool ssNotInstalledDialogVisisble() const;
    void setSsNotInstalledDialogVisisble(bool newSsNotInstalledDialogVisisble);

    void setSteamPath(const QString &newSteamPath);

    bool steamNotInstalledDialogVisisble() const;
    void setSteamNotInstalledDialogVisisble(bool newSteamNotInstalledDialogVisisble);

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

    void sendExit();
    void sendLaunchSoulstorm();
    void noFogStateChanged(bool);

    void sizeModiferChanged(float sizeModifer);
    void sizeModiferLoadedFromSettings(float scale);

    void expandButtonPressed();
    void updateNotification();
    void notificationVisibleChanged();

    void rankedModeStateChanged(bool trainingMode);
    void enableTrainingModeSwitchChanged();
    void onlineCountChanged();

    void currentModNameChanged();
    void currentModTechnicalNameChanged();
    void balanceModPageChanged();

    void notificationManagerChanged();

    void latesBalanceModNotInstalledDialogVisibleChanged();
    void ssNotInstalledDialogVisisbleChanged();
    void steamNotInstalledDialogVisisbleChanged();

    void soulstormIsInstalledChanged();

public slots:
    void expandKeyPressed();
    void expandPatyStatisticButtonClick();

    void receiveSsMaximized(bool maximized);
    void onSsLaunchStateChanged(bool state);

    void setMissionCurrentState(SsMissionState gameCurrentState);

    void receiveNotification(QString notify, bool isWarning);

    void receiveOnlineCount(int onlineCount);

    void receiveCurrentModName(QString modName);
    void receiveCurrentModTechnicalName(QString modName);

    Q_INVOKABLE void onExit();
    Q_INVOKABLE void setSizeModifer(double size);

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
    ReplayManager* m_replayManager;
    MapManagerPage* m_mapManagerPage;
    ModsPage* m_modsPage;
    OnlineStatisticPanel* m_onlineStatisticPanel;
    BalanceModPage* m_balanceModPage;
    NotificationManager* m_notificationManager;

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

    bool m_rankedModeState = true;
    bool m_enableTrainingModeSwitch = true;

    bool m_ssNotInstalledDialogVisisble = false;
    bool m_steamNotInstalledDialogVisisble = false;

    double m_sizeModifer = 1.0;

    QString m_lastNotification;
    bool m_lastNotificationIsWarning = false;
    bool m_notificationVisible = false;

    int m_onlineCount = 0;

    QString m_currentModName = "";
    QString m_currentModTechnicalName = "";
    QString m_ssPath = "";
    QString m_steamPath = "";

    bool m_latesBalanceModNotInstalledDialogVisible = false;
};

#endif // UIBACKEND_H
