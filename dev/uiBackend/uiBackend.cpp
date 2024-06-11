#include <uiBackend.h>
#include <QDebug>
#include <version.h>
#include <QFile>
#include <QDir>

UiBackend::UiBackend(Core* core, QObject *parent)
    : QObject(parent)
    , m_corePtr(core)
    , m_imageProvider(new ImageProvider(this))
    , m_gamePanel(new GamePanel(core->settingsController(), this))
    , m_statisticPanel(new StatisticPanel(m_imageProvider, this))
    , m_newsPage(new MessagesPage(this))
    , m_eventsPage(new MessagesPage(this))
    , m_settingsPageModel(new SettingsPageModel(core->settingsController(), this))
    , m_replayManager(new ReplayManager(m_imageProvider, this))
    , m_mapManagerPage(new MapManagerPage(core->settingsController(), m_imageProvider, this))
    , m_modsPage(new ModsPage(core->settingsController(), this))
    , m_onlineStatisticPanel(new OnlineStatisticPanel(this))
    , m_balanceModPage(new BalanceModPage(core->balanceModManager(), core->settingsController(), this))
    , m_notificationManager(new NotificationManager(this))
    , m_informationPage(new InformationPage(this))
    , m_notificationVisibleTimer(new QTimer(this))  
{
    m_ssStatsVersion.append(PROJECT_VERSION_MAJOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(PROJECT_VERSION_MINOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(GIT_REL);

    m_notificationVisibleTimer->setInterval(7000);
    m_notificationVisibleTimer->setSingleShot(true);

    QObject::connect(core->settingsController(), &SettingsController::settingsLoaded, this, &UiBackend::onSettingsLoaded, Qt::QueuedConnection);
    QObject::connect(m_notificationVisibleTimer, &QTimer::timeout, this, [=]{setNotificationVisible(false);}, Qt::QueuedConnection);

    QObject::connect(m_balanceModPage, &BalanceModPage::changeLaunchMod, this,  [=](LaunchMod launchMod){m_settingsPageModel->setLaunchMode(launchMod);}, Qt::QueuedConnection);
    QObject::connect(m_balanceModPage, &BalanceModPage::sendNotification, m_notificationManager, &NotificationManager::receiveNotification, Qt::QueuedConnection);

    QObject::connect(m_imageProvider,      &ImageProvider::updateAttachments,              m_newsPage,            &MessagesPage::onAttachmetImagesUpdate,           Qt::QueuedConnection);
    QObject::connect(m_imageProvider,      &ImageProvider::updateAttachments,              m_eventsPage,          &MessagesPage::onAttachmetImagesUpdate,           Qt::QueuedConnection);
    QObject::connect(m_imageProvider,      &ImageProvider::updateYoutubeImages,            m_newsPage,            &MessagesPage::onYoutubeImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_imageProvider,      &ImageProvider::updateYoutubeImages,            m_eventsPage,          &MessagesPage::onYoutubeImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_imageProvider,      &ImageProvider::updateAvatars,                  m_newsPage,            &MessagesPage::onAvatarUpdate,                    Qt::QueuedConnection);
    QObject::connect(m_imageProvider,      &ImageProvider::updateAvatars,                  m_eventsPage,          &MessagesPage::onAvatarUpdate,                    Qt::QueuedConnection);

    QObject::connect(m_corePtr->balanceModManager(), &BalanceModManager::sendModsInfo, m_replayManager, &ReplayManager::receiveModsInfo, Qt::QueuedConnection);
}

void UiBackend::expandKeyPressed()
{
    if (m_gameCurrentState != SsMissionState::gameLoadStarted
            && m_gameCurrentState != SsMissionState::playbackLoadStarted
            && m_gameCurrentState != SsMissionState::savedGameLoadStarted
     )
    {
        setExpand(!m_expand);
        emit expandButtonPressed();
    }
}

void UiBackend::expandPatyStatisticButtonClick()
{
    m_patyStatisticVisibleButtonPressedState = !m_patyStatisticVisibleButtonPressedState;

    m_statisticPanel->setExpandPatyStatistic(m_patyStatisticVisibleButtonPressedState);

    m_headerVisible = !m_headerVisible;
    emit headerPanelVisibleChanged();
}

void UiBackend::receiveSsMaximized(bool maximized)
{
    m_ssMaximized = maximized;
    setExpand(false);
    showClient();
}

void UiBackend::onSsLaunchStateChanged(bool state)
{
    //m_ssLaunchState = state;
    setSsLaunchState(state);
    setExpand(false);
    showClient();
}

void UiBackend::loadStarted()
{
    m_loadStarted = true;
    m_gamePanel->onGameStopped();
    m_statisticPanel->setBlockUpdate(true);

    setEnableTrainingModeSwitch(false);

    m_headerVisible = false;
    m_patyStatisticVisible = false;
    m_expand = false;

    if(m_settingsPageModel->overlayVisible())
        emit sendExpand(m_expand);

    emit headerPanelVisibleChanged();
    emit patyStatisticVisibleChanged();
}

void UiBackend::startingMission(SsMissionState gameCurrentState)
{
    m_gamePanel->onGameStarted(gameCurrentState);

    m_missionStarted = true;
    m_headerVisible = false;
    m_patyStatisticVisible = false;

    emit headerPanelVisibleChanged();
    emit patyStatisticVisibleChanged();
}

void UiBackend::gameOver()
{
    startingMission(SsMissionState::gameOver);
}

const QString &UiBackend::softwareUseBanReason() const
{
    return m_softwareUseBanReason;
}

void UiBackend::setSoftwareUseBanReason(const QString &newSoftwareUseBanReason)
{
    if (m_softwareUseBanReason == newSoftwareUseBanReason)
        return;
    m_softwareUseBanReason = newSoftwareUseBanReason;
    emit softwareUseBanReasonChanged();
}

bool UiBackend::softwareUseBanDialogVisible() const
{
    return m_softwareUseBanDialogVisible;
}

void UiBackend::setSoftwareUseBanDialogVisible(bool newSoftwareUseBanDialogVisible)
{
    if (m_softwareUseBanDialogVisible == newSoftwareUseBanDialogVisible)
        return;
    m_softwareUseBanDialogVisible = newSoftwareUseBanDialogVisible;
    emit softwareUseBanDialogVisibleChanged();
}

bool UiBackend::balanceModInstallProcessedDialogVisible() const
{
    return m_balanceModInstallProcessedDialogVisible;
}

void UiBackend::setBalanceModInstallProcessedDialogVisible(bool newBalanceModInstallProcessedDialogVisible)
{
    if (m_balanceModInstallProcessedDialogVisible == newBalanceModInstallProcessedDialogVisible)
        return;
    m_balanceModInstallProcessedDialogVisible = newBalanceModInstallProcessedDialogVisible;
    emit balanceModInstallProcessedDialogVisibleChanged();
}

InformationPage *UiBackend::informationPage() const
{
    return m_informationPage;
}

bool UiBackend::ssLaunchState() const
{
    return m_ssLaunchState;
}

void UiBackend::setSsLaunchState(bool newSsLaunchState)
{
    if (m_ssLaunchState == newSsLaunchState)
        return;
    m_ssLaunchState = newSsLaunchState;
    emit ssLaunchStateChanged();
}

bool UiBackend::getSoulstormLaunchedDialogVisible() const
{
    return m_soulstormLaunchedDialogVisible;
}

void UiBackend::setSoulstormLaunchedDialogVisible(bool newSoulstormLaunchedDialogVisible)
{
    if (m_soulstormLaunchedDialogVisible == newSoulstormLaunchedDialogVisible)
        return;
    m_soulstormLaunchedDialogVisible = newSoulstormLaunchedDialogVisible;
    emit soulstormLaunchedDialogVisibleChanged();
}

bool UiBackend::steamNotInstalledDialogVisible() const
{
    return m_steamNotInstalledDialogVisible;
}

void UiBackend::setSteamNotInstalledDialogVisible(bool newSteamNotInstalledDialogVisible)
{
    if (m_steamNotInstalledDialogVisible == newSteamNotInstalledDialogVisible)
        return;
    m_steamNotInstalledDialogVisible = newSteamNotInstalledDialogVisible;
    emit steamNotInstalledDialogVisibleChanged();
}

void UiBackend::setSteamPath(const QString &newSteamPath)
{
    m_steamPath = newSteamPath;
}

bool UiBackend::ssNotInstalledDialogVisible() const
{
    return m_ssNotInstalledDialogVisible;
}

void UiBackend::setSsNotInstalledDialogVisible(bool newSsNotInstalledDialogVisible)
{
    if (m_ssNotInstalledDialogVisible == newSsNotInstalledDialogVisible)
        return;
    m_ssNotInstalledDialogVisible = newSsNotInstalledDialogVisible;
    emit ssNotInstalledDialogVisibleChanged();
}

void UiBackend::setSsPath(const QString &newSsPath)
{
    if(m_ssPath == newSsPath)
        return;

    m_ssPath = newSsPath;
    emit soulstormIsInstalledChanged();
}

bool UiBackend::latesBalanceModNotInstalledDialogVisible() const
{
    return m_latesBalanceModNotInstalledDialogVisible;
}

void UiBackend::setLatesBalanceModNotInstalledDialogVisible(bool newLatesBalanceModNotInstalledDialogVisible)
{
    if (m_latesBalanceModNotInstalledDialogVisible == newLatesBalanceModNotInstalledDialogVisible)
        return;
    m_latesBalanceModNotInstalledDialogVisible = newLatesBalanceModNotInstalledDialogVisible;
    emit latesBalanceModNotInstalledDialogVisibleChanged();
}

BalanceModPage *UiBackend::balanceModPage() const
{
    return m_balanceModPage;
}

OnlineStatisticPanel *UiBackend::onlineStatisticPanel() const
{
    return m_onlineStatisticPanel;
}

ModsPage *UiBackend::modsPage() const
{
    return m_modsPage;
}

const QString UiBackend::currentModTechnicalName() const
{
    if(m_currentModTechnicalName.toLower().contains("dowstats_balance_mod"))
        return "dowstats_balance_mod";
    else
        return m_currentModTechnicalName;
}

void UiBackend::setCurrentModTechnicalName(const QString &newCurrentModTechnicalName)
{
    if (m_currentModTechnicalName == newCurrentModTechnicalName)
        return;
    m_currentModTechnicalName = newCurrentModTechnicalName;
    emit currentModTechnicalNameChanged();
}

const QString &UiBackend::currentModName() const
{
    return m_currentModName;
}

void UiBackend::setCurrentModName(const QString &newCurrentModName)
{
    if (m_currentModName == newCurrentModName)
        return;
    m_currentModName = newCurrentModName;
    emit currentModNameChanged();
}

MapManagerPage *UiBackend::mapManagerPage() const
{
    return m_mapManagerPage;
}

int UiBackend::onlineCount() const
{
    return m_onlineCount;
}

void UiBackend::setOnlineCount(int newOnlineCount)
{
    if (m_onlineCount == newOnlineCount)
        return;
    m_onlineCount = newOnlineCount;
    emit onlineCountChanged();
}

void UiBackend::determinateRankedModePanelVisible()
{
    if (m_gameCurrentState == SsMissionState::playbackLoadStarted
        || m_gameCurrentState == SsMissionState::playbackStarted
        || m_gameCurrentState == SsMissionState::playbackOver
        || m_gameCurrentState == SsMissionState::savedGameLoadStarted
        || m_gameCurrentState == SsMissionState::savedGameStarted
        || m_gameCurrentState == SsMissionState::savedGameOver
        || m_gameCurrentState == SsMissionState::unknown
        || m_gameCurrentState == SsMissionState::unknownGameStarted
        || m_gameCurrentState == SsMissionState::unknownGameOver
            )
        m_gamePanel->setRankedModePanelVisible(false);
    else
        m_gamePanel->setRankedModePanelVisible(true);
}

bool UiBackend::enableTrainingModeSwitch() const
{
    return m_enableTrainingModeSwitch;
}

void UiBackend::setEnableTrainingModeSwitch(bool newEnableTrainingModeSwitch)
{
    if (m_enableTrainingModeSwitch == newEnableTrainingModeSwitch)
        return;
    m_enableTrainingModeSwitch = newEnableTrainingModeSwitch;
    emit enableTrainingModeSwitchChanged();
}

bool UiBackend::rankedModeState() const
{
    return m_rankedModeState;
}

void UiBackend::setRankedModeState(bool newRankedModeState)
{
    if (m_rankedModeState == newRankedModeState)
        return;
    m_rankedModeState = newRankedModeState;
    emit rankedModeStateChanged(m_rankedModeState);
}

ReplayManager *UiBackend::replayManager() const
{
    return m_replayManager;
}

bool UiBackend::notificationVisible() const
{
    return m_notificationVisible;
}

void UiBackend::setNotificationVisible(bool newNotificationVisible)
{
    if(m_notificationVisible == newNotificationVisible)
        return;

    m_notificationVisible = newNotificationVisible;
    emit notificationVisibleChanged();
}

void UiBackend::setMissionCurrentState(SsMissionState gameCurrentState)
{
    if (m_gameCurrentState == gameCurrentState)
        return;

    m_gameCurrentState = gameCurrentState;

    //TODO: Тут нужен рефактор, желательн о перевести взаимодействие с гейминфоридером на один сигнал присылающий текущее состояние.


    switch (m_gameCurrentState)
    {
        case SsMissionState::gameLoadStarted :
        case SsMissionState::playbackLoadStarted :
        case SsMissionState::savedGameLoadStarted : loadStarted(); break;

        case SsMissionState::gameStoped :
        case SsMissionState::playbackStoped :
        case SsMissionState::savedGameStoped :
        case SsMissionState::unknownGameStoped : gameStopped(); break;

        case SsMissionState::gameStarted :
        case SsMissionState::playbackStarted :
        case SsMissionState::savedGameStarted :
        case SsMissionState::unknownGameStarted : startingMission(m_gameCurrentState); break;

        case SsMissionState::gameOver :
        case SsMissionState::playbackOver :
        case SsMissionState::savedGameOver :
        case SsMissionState::unknownGameOver : gameOver(); break;

        default: break;
    }

    determinateRankedModePanelVisible();
}

void UiBackend::receiveNotification(QString notify, bool isWarning)
{
    m_lastNotification = "";
    emit updateNotification();
    m_lastNotification = notify;
    m_lastNotificationIsWarning = isWarning;
    emit updateNotification();
    m_notificationVisibleTimer->start();
    setNotificationVisible(true);
}

void UiBackend::receiveOnlineCount(int onlineCount)
{
    m_onlineCount = onlineCount;
    emit onlineCountChanged();
}

void UiBackend::receiveCurrentModName(QString modName)
{
    setCurrentModName(modName);
}

void UiBackend::receiveCurrentModTechnicalName(QString modName)
{
    setCurrentModTechnicalName(modName);
}

void UiBackend::receiveActualClientVersion(QString version)
{
    if(version > m_ssStatsVersion && !m_clientUpdateAvailable)
    {
        m_clientUpdateAvailable = true;

        NotificationInfo notificationInfo;

        notificationInfo.text = tr("Client update version ") + version + tr(" is available, restart the client to install updates.");
        notificationInfo.type = NotificationType::Update;

        m_notificationManager->receiveNotification(notificationInfo);
    }
}

void UiBackend::onSoftwareBanActivated(QString reason)
{
    if (m_softwareBanActivated)
        return;

    m_softwareBanActivated = true;

    setSoftwareUseBanDialogVisible(true);
    setSoftwareUseBanReason(reason);

    m_settingsPageModel->setOverlayVisible(false);

}

void UiBackend::onExit()
{
    emit sendExit();
}

void UiBackend::launchSoulstorm()
{
    QDir steamDir(m_steamPath);

    if(!soulstormIsInstalled())
        setSsNotInstalledDialogVisible(true);
    else if(m_steamPath.isEmpty() || !steamDir.exists())
        setSteamNotInstalledDialogVisible(true);
    else if (m_settingsPageModel->launchMode() == LaunchMod::DowStatsBalanceMod && !m_balanceModPage->isLatestModInstalled())
        setLatesBalanceModNotInstalledDialogVisible(true);
    else if (m_balanceModPage->downloadingProcessed())
        setBalanceModInstallProcessedDialogVisible(true);
    else
        emit sendLaunchSoulstorm();
}

void UiBackend::setSizeModifer(double size)
{
    m_sizeModifer = size;

    emit sizeModiferChanged(m_sizeModifer);

    m_corePtr->settingsController()->getSettings()->scale = m_sizeModifer;
    m_corePtr->settingsController()->saveSettings();
}

void UiBackend::onSettingsLoaded()
{
    m_noFogState = m_corePtr->settingsController()->getSettings()->noFog;
    emit noFogStateChanged(m_noFogState);
    m_sizeModifer = m_corePtr->settingsController()->getSettings()->scale;
    emit sizeModiferLoadedFromSettings(m_sizeModifer);
    emit sizeModiferChanged(m_sizeModifer);

    setCurrentModTechnicalName(m_corePtr->settingsController()->getSettings()->currentMod);

}

void UiBackend::showClient()
{
    m_showClient = m_ssLaunchState && m_ssMaximized;
    emit sendShowClient(m_showClient);
}

SettingsPageModel *UiBackend::settingsPageModel() const
{
    return m_settingsPageModel;
}

bool UiBackend::soulstormIsInstalled()
{
    QFile ssDir(m_ssPath + QDir::separator() + "Soulstorm.exe");

    return !m_ssPath.isEmpty() && ssDir.exists();
}

MessagesPage *UiBackend::eventsPage() const
{
    return m_eventsPage;
}

MessagesPage *UiBackend::newsPage() const
{
    return m_newsPage;
}

bool UiBackend::getFogState() const
{
    return m_noFogState;
}

void UiBackend::onMouseWheel(int delta)
{
    emit sendMouseWheel(delta);
}

void UiBackend::setNoFogState(bool state)
{
    m_noFogState = state;
    m_corePtr->settingsController()->getSettings()->noFog = m_noFogState;
    m_corePtr->settingsController()->saveSettings();
    emit noFogStateChanged(m_noFogState);
}

ImageProvider *UiBackend::imageProvider() const
{
    return m_imageProvider;
}

GamePanel *UiBackend::gamePanel() const
{
    return m_gamePanel;
}

StatisticPanel *UiBackend::statisticPanel() const
{
    return m_statisticPanel;
}

void UiBackend::setSsWindowed(bool newSsWindowed)
{
    m_ssWindowed = newSsWindowed;
    emit ssWindowedModeChanged();

    if(m_ssWindowed)
        emit windowedModeSeted();
}

void UiBackend::setSsWindowPosition(int x, int y)
{
    m_ssWindowPositionX = x;
    m_ssWindowPositionY = y;

    emit ssWindowPositionChanged();
}

bool UiBackend::getShowClient()
{
    return m_showClient;
}


bool UiBackend::expand() const
{
    return m_expand;
}

void UiBackend::setExpand(bool newExpand)
{
    m_expand = newExpand;

    if(m_settingsPageModel->overlayVisible())
        emit sendExpand(m_expand);

    if (m_missionStarted)
    {
        if (m_expand)
        {
            m_gamePanel->setGamePanelVisible(true);
            m_headerVisible = true;
            m_patyStatisticVisible = true;
            m_statisticPanel->setExpandPatyStatistic(false);
        }
        else
        {
            m_gamePanel->setGamePanelVisible(true);
            m_headerVisible = false;
            m_patyStatisticVisible = false;
            m_statisticPanel->setExpandPatyStatistic(true);
        }

        emit headerPanelVisibleChanged();
        emit patyStatisticVisibleChanged();
    }
    else
    {
        if(m_patyStatisticVisibleButtonPressedState)
        {
             if (m_expand)
                m_headerVisible = true;
             else
                m_headerVisible = false;

             emit headerPanelVisibleChanged();
        }
    }
}

void UiBackend::mousePressEvent(QPoint mousePosition)
{
    m_mousePosition = mousePosition;
    emit sendMousePress();
}

void UiBackend::mouseMoveEvent(QPoint mousePosition)
{
    m_mousePosition = mousePosition;
    emit sendMouseMove();
}

int UiBackend::mousePositionX()
{
    return m_mousePosition.x();
}

int UiBackend::mousePositionY()
{
    return m_mousePosition.y();
}

void UiBackend::setWindowTopmost(bool topmost)
{
    m_windowTopmost = topmost;
    emit windowTopmostChanged();
}

bool UiBackend::getWindowTopmost()
{
    return m_windowTopmost;
}

void UiBackend::setMouseArea(int width, int height)
{
    m_mouseAreaWidth = width;
    m_mouseAreaHeight = height;
}

void UiBackend::gameStopped()
{
    m_loadStarted = false;
    m_missionStarted = false;
    m_gamePanel->onGameStopped();
    m_statisticPanel->setBlockUpdate(false);

    setEnableTrainingModeSwitch(true);

    if(m_patyStatisticVisibleButtonPressedState)
    {
        m_headerVisible = false;
        m_statisticPanel->setExpandPatyStatistic(true);
    }
    else
    {
        m_headerVisible = true;
        m_statisticPanel->setExpandPatyStatistic(false);
    }

    m_patyStatisticVisible = true;

    emit headerPanelVisibleChanged();
    emit patyStatisticVisibleChanged();
}
