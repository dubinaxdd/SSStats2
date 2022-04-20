#include <uibackend.h>
#include <QDebug>
#include <version.h>
#include <settingscontroller.h>

UiBackend::UiBackend(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_imageProvider(new ImageProvider(this))
    , m_gamePanel(new GamePanel(settingsController, this))
    , m_statisticPanel(new StatisticPanel(m_imageProvider, this))
    , m_newsPage(new MessagesPage(this))
    , m_eventsPage(new MessagesPage(this))
    , m_settingsPageModel(new SettingsPageModel(this))
{
    m_ssStatsVersion.append(PROJECT_VERSION_MAJOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(PROJECT_VERSION_MINOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(GIT_REL);

    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &UiBackend::onSettingsLoaded, Qt::QueuedConnection);

    emit gamePanelInitialized();
    emit statisticPanelInitialized();
    emit statsHeaderInitialized();
}

void UiBackend::expandKeyPressed()
{
    if (m_gameCurrentState != SsGameState::gameLoadStarted
            && m_gameCurrentState != SsGameState::playbackLoadStarted
            && m_gameCurrentState != SsGameState::savedGameLoadStarted
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
    m_ssLaunchState = state;
    setExpand(false);
    showClient();
}

void UiBackend::onLoadStarted()
{
    m_loadStarted = true;
    m_gamePanel->onGameStopped();
    m_statisticPanel->setBlockUpdate(true);

    m_headerVisible = false;
    m_patyStatisticVisible = false;

    emit headerPanelVisibleChanged();
    emit patyStatisticVisibleChanged();
}

void UiBackend::onStartingMission(SsGameState gameCurrentState)
{
    m_gamePanel->onGameStarted(gameCurrentState);

    m_missionStarted = true;

    m_headerVisible = false;
    m_patyStatisticVisible = false;

    emit headerPanelVisibleChanged();
    emit patyStatisticVisibleChanged();
}

void UiBackend::onGameOver()
{
    onStartingMission(SsGameState::gameOver);
}

void UiBackend::setGameCurrentState(SsGameState gameCurrentState)
{
    if (m_gameCurrentState == gameCurrentState)
        return;

    m_gameCurrentState = gameCurrentState;

    //TODO: Тут нужен рефактор, желательн о перевести взаимодействие с гейминфоридером на один сигнал присылающий текущее состояние.

/*
    if (m_gameCurrentState == SsGameState::gameLoadStarted
            || m_gameCurrentState == SsGameState::playbackLoadStarted
            || m_gameCurrentState == SsGameState::savedGameLoadStarted
     )
        onLoadStarted();


    if (m_gameCurrentState == SsGameState::gameStoped)
        onGameStopped();

    if (m_gameCurrentState ==SsGameState::gameStarted
            || m_gameCurrentState ==SsGameState::playbackStarted
            || m_gameCurrentState ==SsGameState::savedGameStarted
            )
    {
        onStartingMission(m_gameCurrentState);
    }

    if (m_gameCurrentState == SsGameState::gameStoped)
        onGameStopped();


    if (m_gameCurrentState == SsGameState::gameOver)
        onGameOver();*/
}

void UiBackend::receiveNotification(QString notify)
{
    m_lastNotification = "";
    emit updateNotification();
    m_lastNotification = notify;
    emit updateNotification();
}

void UiBackend::onExit()
{
    emit sendExit();
}

void UiBackend::onLaunchSoulstormWithSupportMode()
{
    emit sendLaunchSoulstormWithSupportMode();
}

void UiBackend::launchSoulstorm()
{
    emit sendLaunchSoulstorm();
}

void UiBackend::setSizeModifer(float size)
{
    m_sizeModifer = size;

    emit sizeModiferChanged(m_sizeModifer);

    m_settingsController->getSettings()->scale = m_sizeModifer;
    m_settingsController->saveSettings();
}

void UiBackend::onSettingsLoaded()
{
    m_noFogState = m_settingsController->getSettings()->noFog;
    emit noFogStateChanged(m_noFogState);
    m_sizeModifer = m_settingsController->getSettings()->scale;
    emit sizeModiferLoadedFromSettings(m_sizeModifer);
    emit sizeModiferChanged(m_sizeModifer);
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
    m_settingsController->getSettings()->noFog = m_noFogState;
    m_settingsController->saveSettings();
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
    emit sendExpand(m_expand);

    if (m_missionStarted)
    {
        if (m_expand)
        {
            m_gamePanel->setGamePanelVisisble(true);
            m_headerVisible = true;
            m_patyStatisticVisible = true;
            m_statisticPanel->setExpandPatyStatistic(false);
        }
        else
        {
            m_gamePanel->setGamePanelVisisble(true);
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

void UiBackend::onGameStopped()
{
    m_loadStarted = false;
    m_missionStarted = false;
    m_gamePanel->onGameStopped();
    m_statisticPanel->setBlockUpdate(false);

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
