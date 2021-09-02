#include "uibackend.h"
#include "QDebug"
#include "../../version.h"

UiBackend::UiBackend(QObject *parent)
    : QObject(parent)
    , m_imageProvider(new ImageProvider(this))
    , m_gamePanel(new GamePanel(this))
    , m_statisticPanel(new StatisticPanel(m_imageProvider, this))
{
    m_ssStatsVersion.append(PROJECT_VERSION_MAJOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(PROJECT_VERSION_MINOR);
    m_ssStatsVersion.append(".");
    m_ssStatsVersion.append(GIT_REL);

    emit gamePanelInitialized();
    emit statisticPanelInitialized();
    emit statsHeaderInitialized();
}

void UiBackend::expandKeyPressed()
{
    setExpand(!m_expand);
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
    emit headerPanelVisibleChanged(false);
}

void UiBackend::onStartingMission()
{
    m_missionStarted = true;
}

void UiBackend::showClient()
{
    m_showClient = m_ssLaunchState && m_ssMaximized;
    emit sendShowClient(m_showClient);
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

    if(m_ssWindowed)
        emit windowedModeSeted();
}

void UiBackend::setSsWindowPosition(int x, int y)
{
    m_ssWindowPositionX = x;
    m_ssWindowPositionY = y;

    emit ssWindowPositionChanged();
}

// ### GET раздел (частично) ###

bool UiBackend::getShowClient()
{
    return m_showClient;
}

bool UiBackend::expand() const
{
    return m_expand;
}

void UiBackend::onNoFogStateChanged(bool state)
{
    emit noFogStateChanged(state); // Отправка значения No Fog в QML
}

void UiBackend::onSwitchNoFogStateChanged(bool state) // Это слот, для вызова инициализатором настроек извне и в обход меню настроек GUI
{
    emit switchNoFogStateChanged(state);
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
            emit headerPanelVisibleChanged(true);
        }
        else
        {
            m_gamePanel->setGamePanelVisisble(true);
            emit headerPanelVisibleChanged(false);
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
    emit headerPanelVisibleChanged(true);
}
