#include "uibackend.h"

UiBackend::UiBackend(QObject *parent) : QObject(parent)
{

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

void UiBackend::receiveSsLounched(bool lounched)
{
    m_ssLounched = lounched;
    setExpand(false);
    showClient();
}

void UiBackend::receivePlayersTestStats(QVector<PlayerStats> testStats)
{
    if(testStats.count() < 8)
        return;

    m_player0Race = "Team " + QString::number(testStats.at(0).team.toInt() + 1) + ": " + testStats.at(0).name + " - " + testStats.at(0).race;
    m_player1Race = "Team " + QString::number(testStats.at(1).team.toInt() + 1) + ": " + testStats.at(1).name + " - " + testStats.at(1).race;
    m_player2Race = "Team " + QString::number(testStats.at(2).team.toInt() + 1) + ": " + testStats.at(2).name + " - " + testStats.at(2).race;
    m_player3Race = "Team " + QString::number(testStats.at(3).team.toInt() + 1) + ": " + testStats.at(3).name + " - " + testStats.at(3).race;
    m_player4Race = "Team " + QString::number(testStats.at(4).team.toInt() + 1) + ": " + testStats.at(4).name + " - " + testStats.at(4).race;
    m_player5Race = "Team " + QString::number(testStats.at(5).team.toInt() + 1) + ": " + testStats.at(5).name + " - " + testStats.at(5).race;
    m_player6Race = "Team " + QString::number(testStats.at(6).team.toInt() + 1) + ": " + testStats.at(6).name + " - " + testStats.at(6).race;
    m_player7Race = "Team " + QString::number(testStats.at(7).team.toInt() + 1) + ": " + testStats.at(7).name + " - " + testStats.at(7).race;


    replaceRaceKeyword(&m_player0Race);
    replaceRaceKeyword(&m_player1Race);
    replaceRaceKeyword(&m_player2Race);
    replaceRaceKeyword(&m_player3Race);
    replaceRaceKeyword(&m_player4Race);
    replaceRaceKeyword(&m_player5Race);
    replaceRaceKeyword(&m_player6Race);
    replaceRaceKeyword(&m_player7Race);

    emit playerTestStatsUpdate();
}

void UiBackend::gameStarted()
{
    m_gamePanelVisible = true;
    emit gamePanelVisibleChanged(m_gamePanelVisible);
}

void UiBackend::gameStoped()
{
    m_player0Race = "";
    m_player1Race = "";
    m_player2Race = "";
    m_player3Race = "";
    m_player4Race = "";
    m_player5Race = "";
    m_player6Race = "";
    m_player7Race = "";

    emit playerTestStatsUpdate();

    m_gamePanelVisible = false;
    emit gamePanelVisibleChanged(m_gamePanelVisible);
}

void UiBackend::showClient()
{
    m_showClient = m_ssLounched && m_ssMaximized;
    emit sendShowClient(m_showClient);
}

void UiBackend::replaceRaceKeyword(QString *raceString)
{
    if (raceString->contains("guard_race"))
        raceString->replace("guard_race", "Imperial Guard");

    if (raceString->contains("tau_race"))
        raceString->replace("tau_race", "Tau Empire");

    if (raceString->contains("eldar_race"))
        raceString->replace("eldar_race", "Eldar");

    if (raceString->contains("ork_race"))
        raceString->replace("ork_race", "Orks");

    if (raceString->contains("chaos_marine_race"))
        raceString->replace("chaos_marine_race", "Chaos");

    if (raceString->contains("necron_race"))
        raceString->replace("necron_race", "Necrons");

    if (raceString->contains("space_marine_race"))
        raceString->replace("space_marine_race", "Space Marines");

    if (raceString->contains("dark_eldar_race"))
        raceString->replace("dark_eldar_race", "Dark Eldar");

    if (raceString->contains("sisters_race"))
        raceString->replace("sisters_race", "Sisters of Battle");

}

bool UiBackend::expand() const
{
    return m_expand;
}

bool UiBackend::getShowClient()
{
    return m_showClient;
}

void UiBackend::setExpand(bool newExpand)
{
    m_expand = newExpand;
    emit sendExpand(m_expand);
}

void UiBackend::mousePressEvent(QPoint mousePosition)
{
    m_mousePosition = mousePosition;
    emit sendMousePress();
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

void UiBackend::setWindowedMode()
{
    emit windowedModeSeted();
}
