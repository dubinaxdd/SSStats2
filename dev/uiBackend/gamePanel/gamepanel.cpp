#include "gamepanel.h"
#include <QDebug>

#define GAME_LEAVE_TIMER_INTERVAL 1000

GamePanel::GamePanel(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    m_racePanelVisibleTimer = new QTimer(this);
    m_racePanelVisibleTimer->setInterval(20000);
    m_racePanelVisibleTimer->setSingleShot(true);

    m_gameLeaveTimer = new QTimer(this);
    m_gameLeaveTimer->setInterval(GAME_LEAVE_TIMER_INTERVAL);


    QObject::connect(m_racePanelVisibleTimer, &QTimer::timeout, this, &GamePanel::racePanelVisibleTimerTimeout, Qt::QueuedConnection);
    QObject::connect(m_gameLeaveTimer, &QTimer::timeout, this, &GamePanel::gameLeaveTimerTimeout, Qt::QueuedConnection);

    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &GamePanel::onSettingsLoaded, Qt::QueuedConnection);


}

void GamePanel::racePanelVisibleTimerTimeout()
{
    m_racePanelVisisble = false;
    emit racePanelVisibleChanged(m_racePanelVisisble);
}

void GamePanel::gameLeaveTimerTimeout()
{
    m_gameLeaveTimeLeft--;

    if(m_gameLeaveTimeLeft < -5)
    {

        m_gameLeaveTimer->stop();
        m_gameLeaveTimerVisible = false;
        emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
    }
    else if (m_gameLeaveTimeLeft >= 0)
    {
        emit gemeLeaveTimeLeftChanged(m_gameLeaveTimeLeft);
    }

}

void GamePanel::onApmChanged(int capm, int aapm)
{
    m_currentApm = QString::number(capm);
    m_averageApm = QString::number(aapm);
    emit apmUpdate();
}

void GamePanel::onGameStopped()
{
    m_gameLeaveTimer->stop();

    m_currentApm = QString("-");
    m_averageApm = QString("-");
    emit apmUpdate();

    m_player0Race = "";
    m_player1Race = "";
    m_player2Race = "";
    m_player3Race = "";
    m_player4Race = "";
    m_player5Race = "";
    m_player6Race = "";
    m_player7Race = "";

    emit playerTestStatsUpdate();

    m_gamePanelVisisble = false;
    emit gamePanelVisibleChanged(m_gamePanelVisisble);
    m_racePanelVisisble = false;
    emit racePanelVisibleChanged(m_racePanelVisisble);
}

void GamePanel::onGameStarted(SsGameState gameCurrentState)
{
    if (gameCurrentState == SsGameState::unknownGameStarted || gameCurrentState == SsGameState::playbackStarted)
    {
        m_currentApm = QString("-");
        m_averageApm = QString("-");
        emit apmUpdate();
    }

    if (gameCurrentState != SsGameState::gameStarted)
    {
        m_gameLeaveTimerVisible = false;
        emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
        return;
    }

    m_gamePanelVisisble = true;
    emit gamePanelVisibleChanged(m_gamePanelVisisble);

    m_gameLeaveTimeLeft = 30;
    emit gemeLeaveTimeLeftChanged(m_gameLeaveTimeLeft);
    m_gameLeaveTimer->start();
    m_gameLeaveTimerVisible = true;
    m_racePanelVisibleTimer->start();
    emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
}


void GamePanel::receivePlayersTestStats(QVector<PlayerStats> testStats)
{
    m_testStats = testStats;
    m_racePanelVisisble = true;
    emit racePanelVisibleChanged(m_racePanelVisisble);

    updatePlayerRaces();
}

void GamePanel::expandPlayerRacesButtonClick()
{
    m_racePanelVisisble = !m_racePanelVisisble;
    emit racePanelVisibleChanged(m_racePanelVisisble);
}

void GamePanel::onSettingsLoaded()
{
    m_smallPannelActive = m_settingsController->getSettings()->smallGamePanelActive;
    emit smallPannelActiveChanged(m_smallPannelActive);
    m_showGamePannelPreset = m_settingsController->getSettings()->showGamePanelPreset;
    emit showGamePanelPresetChanged(m_showGamePannelPreset);

    updatePlayerRaces();
}

void GamePanel::replaceRaceKeyword(QString *raceString)
{
    if (raceString->contains("guard_race"))
    {
        raceString->replace("guard_race", "Imperial Guard");
        return;
    }

    if (raceString->contains("tau_race"))
    {
        raceString->replace("tau_race", "Tau Empire");
        return;
    }

    if (raceString->contains("ork_race"))
    {
        raceString->replace("ork_race", "Orks");
        return;
    }

    if (raceString->contains("chaos_marine_race"))
    {
        raceString->replace("chaos_marine_race", "Chaos");
        return;
    }

    if (raceString->contains("necron_race"))
    {
        raceString->replace("necron_race", "Necrons");
        return;
    }

    if (raceString->contains("space_marine_race"))
    {
        raceString->replace("space_marine_race", "Space Marines");
        return;
    }

    if (raceString->contains("sisters_race"))
    {
        raceString->replace("sisters_race", "Sisters of Battle");
        return;
    }

    if (raceString->contains("dark_eldar_race"))
    {
        raceString->replace("dark_eldar_race", "Dark Eldar");
        return;
    }

    if (raceString->contains("eldar_race"))
    {
        raceString->replace("eldar_race", "Eldar");
        return;
    }
}

QString GamePanel::chooseColorForPlayer(int team)
{
    switch (team)
    {
        case 0 : return "#b3ea0000";
        case 1 : return "#b30469ee";
        case 2 : return "#b3dacfcf";
        case 3 : return "#b3f6f200";
        case 4 : return "#b30dff1e";
        case 5 : return "#b3f71df4";
        case 6 : return "#b300e6d9";
        case 7 : return "#b3f97dfd";
    }
    return "";
}

void GamePanel::updatePlayerRaces()
{
    if(m_testStats.count() < 8)
        return;

    if (!m_smallPannelActive)
    {
        if(!m_testStats.at(0).name.isEmpty())
            m_player0Race = "Team " + QString::number(m_testStats.at(0).team.toInt() + 1) + ": " + m_testStats.at(0).name + " - " + m_testStats.at(0).race;

        if(!m_testStats.at(1).name.isEmpty())
            m_player1Race = "Team " + QString::number(m_testStats.at(1).team.toInt() + 1) + ": " + m_testStats.at(1).name + " - " + m_testStats.at(1).race;

        if(!m_testStats.at(2).name.isEmpty())
            m_player2Race = "Team " + QString::number(m_testStats.at(2).team.toInt() + 1) + ": " + m_testStats.at(2).name + " - " + m_testStats.at(2).race;

        if(!m_testStats.at(3).name.isEmpty())
            m_player3Race = "Team " + QString::number(m_testStats.at(3).team.toInt() + 1) + ": " + m_testStats.at(3).name + " - " + m_testStats.at(3).race;

        if(!m_testStats.at(4).name.isEmpty())
            m_player4Race = "Team " + QString::number(m_testStats.at(4).team.toInt() + 1) + ": " + m_testStats.at(4).name + " - " + m_testStats.at(4).race;

        if(!m_testStats.at(5).name.isEmpty())
            m_player5Race = "Team " + QString::number(m_testStats.at(5).team.toInt() + 1) + ": " + m_testStats.at(5).name + " - " + m_testStats.at(5).race;

        if(!m_testStats.at(6).name.isEmpty())
            m_player6Race = "Team " + QString::number(m_testStats.at(6).team.toInt() + 1) + ": " + m_testStats.at(6).name + " - " + m_testStats.at(6).race;

        if(!m_testStats.at(7).name.isEmpty())
            m_player7Race = "Team " + QString::number(m_testStats.at(7).team.toInt() + 1) + ": " + m_testStats.at(7).name + " - " + m_testStats.at(7).race;
    }
    else
    {
        if(!m_testStats.at(0).name.isEmpty())
            m_player0Race = m_testStats.at(0).name.left(12) + " - " + m_testStats.at(0).race;

        if(!m_testStats.at(1).name.isEmpty())
            m_player1Race = m_testStats.at(1).name.left(12) + " - " + m_testStats.at(1).race;

        if(!m_testStats.at(2).name.isEmpty())
            m_player2Race = m_testStats.at(2).name.left(12) + " - " + m_testStats.at(2).race;

        if(!m_testStats.at(3).name.isEmpty())
            m_player3Race = m_testStats.at(3).name.left(12) + " - " + m_testStats.at(3).race;

        if(!m_testStats.at(4).name.isEmpty())
            m_player4Race = m_testStats.at(4).name.left(12) + " - " + m_testStats.at(4).race;

        if(!m_testStats.at(5).name.isEmpty())
            m_player5Race = m_testStats.at(5).name.left(12) + " - " + m_testStats.at(5).race;

        if(!m_testStats.at(6).name.isEmpty())
            m_player6Race = m_testStats.at(6).name.left(12) + " - " + m_testStats.at(6).race;

        if(!m_testStats.at(7).name.isEmpty())
            m_player7Race = m_testStats.at(7).name.left(12) + " - " + m_testStats.at(7).race;
    }

    replaceRaceKeyword(&m_player0Race);
    replaceRaceKeyword(&m_player1Race);
    replaceRaceKeyword(&m_player2Race);
    replaceRaceKeyword(&m_player3Race);
    replaceRaceKeyword(&m_player4Race);
    replaceRaceKeyword(&m_player5Race);
    replaceRaceKeyword(&m_player6Race);
    replaceRaceKeyword(&m_player7Race);

    m_player0Color = chooseColorForPlayer(m_testStats.at(0).team.toInt());
    m_player1Color = chooseColorForPlayer(m_testStats.at(1).team.toInt());
    m_player2Color = chooseColorForPlayer(m_testStats.at(2).team.toInt());
    m_player3Color = chooseColorForPlayer(m_testStats.at(3).team.toInt());
    m_player4Color = chooseColorForPlayer(m_testStats.at(4).team.toInt());
    m_player5Color = chooseColorForPlayer(m_testStats.at(5).team.toInt());
    m_player6Color = chooseColorForPlayer(m_testStats.at(6).team.toInt());
    m_player7Color = chooseColorForPlayer(m_testStats.at(7).team.toInt());

    emit playerTestStatsUpdate();
}

const QString &GamePanel::averageApm() const
{
    return m_averageApm;
}

const QString &GamePanel::currentApm() const
{
    return m_currentApm;
}

bool GamePanel::showGamePannelPreset() const
{
    return m_showGamePannelPreset;
}

void GamePanel::setShowGamePannelPreset(bool showGamePannelPreset)
{
    m_showGamePannelPreset = showGamePannelPreset;
    m_settingsController->getSettings()->showGamePanelPreset = m_showGamePannelPreset;
    m_settingsController->saveSettings();
    emit showGamePanelPresetChanged(m_showGamePannelPreset);
}

void GamePanel::setGamePanelVisisble(bool newGamePanelVisisble)
{
    m_gamePanelVisisble = newGamePanelVisisble;
    emit gamePanelVisibleChanged(m_gamePanelVisisble);
}

void GamePanel::setSmallGamePanelActive(bool active)
{
    m_smallPannelActive = active;
    m_settingsController->getSettings()->smallGamePanelActive = active;
    m_settingsController->saveSettings();
    emit smallPannelActiveChanged(active);

    updatePlayerRaces();
}

bool GamePanel::getSmallGamePanelActive()
{
    return m_smallPannelActive;
}

