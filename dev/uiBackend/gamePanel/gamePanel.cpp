#include "gamePanel.h"
#include <QDebug>

#define GAME_LEAVE_TIMER_INTERVAL 1000

GamePanel::GamePanel(SoulstormController *soulstormController, SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_soulstormControllerPtr(soulstormController)
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

    QObject::connect(m_soulstormControllerPtr->gameStateReader(), &GameStateReader::sendPlayersTestStats, this, &GamePanel::receivePlayersTestStats,     Qt::QueuedConnection);
    QObject::connect(m_soulstormControllerPtr->apmMeter(),        &APMMeter::apmCalculated,               this,  &GamePanel::onApmChanged,            Qt::QueuedConnection);
    QObject::connect(m_soulstormControllerPtr->gameStateReader(), &GameStateReader::sendGameRankedMode,   this, [&](bool gameRankedMode){ this->setGameRankedMode(gameRankedMode);},         Qt::QueuedConnection);
}

void GamePanel::racePanelVisibleTimerTimeout()
{
    m_racePanelVisible = false;
    emit racePanelVisibleChanged(m_racePanelVisible);
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
    m_racePanelVisibleTimer->stop();

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

    m_gamePanelVisible = false;
    emit gamePanelVisibleChanged(m_gamePanelVisible);
    m_racePanelVisible = false;
    emit racePanelVisibleChanged(m_racePanelVisible);
}

void GamePanel::onGameStarted(SsMissionState gameCurrentState)
{
    if (gameCurrentState == SsMissionState::unknownGameStarted || gameCurrentState == SsMissionState::playbackStarted)
    {
        m_currentApm = QString("-");
        m_averageApm = QString("-");
        emit apmUpdate();
    }

    if (gameCurrentState != SsMissionState::gameStarted && gameCurrentState != SsMissionState::savedGameStarted && gameCurrentState != SsMissionState::playbackStarted)
    {
        m_gameLeaveTimerVisible = false;
        emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
        return;
    }

    m_gamePanelVisible = true;
    emit gamePanelVisibleChanged(m_gamePanelVisible);

    if (gameCurrentState == SsMissionState::gameStarted)
    {
        m_gameLeaveTimeLeft = 45;
        emit gemeLeaveTimeLeftChanged(m_gameLeaveTimeLeft);
        m_gameLeaveTimer->start();
        m_gameLeaveTimerVisible = true;
        m_racePanelVisibleTimer->start();
        emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
    }
    else
    {
        m_gameLeaveTimerVisible = false;
        m_racePanelVisibleTimer->start();
        emit gameLeaveTimerVisibleChanged(m_gameLeaveTimerVisible);
    }
}


void GamePanel::receivePlayersTestStats(QVector<PlayerStats> testStats)
{
    m_testStats = testStats;
    m_racePanelVisible = true;
    emit racePanelVisibleChanged(m_racePanelVisible);

    updatePlayerRaces();
}

void GamePanel::expandPlayerRacesButtonClick()
{
    m_racePanelVisible = !m_racePanelVisible;
    emit racePanelVisibleChanged(m_racePanelVisible);
}

void GamePanel::onSettingsLoaded()
{
    qInfo(logInfo()) << "GamePanel::onSettingsLoaded()" << "load started";

    m_smallPannelActive = m_settingsController->getSettings()->smallGamePanelActive;
    emit smallPannelActiveChanged(m_smallPannelActive);
    m_showGamePannelPreset = m_settingsController->getSettings()->showGamePanelPreset;
    emit showGamePanelPresetChanged(m_showGamePannelPreset);

    updatePlayerRaces();

    qInfo(logInfo()) << "GamePanel::onSettingsLoaded()" << "load finished";
}

void GamePanel::replaceRaceKeyword(QString *raceString)
{
    if (raceString->contains("guard_race"))
    {
        raceString->replace("guard_race", tr("Imperial Guard"));
        return;
    }

    if (raceString->contains("tau_race"))
    {
        raceString->replace("tau_race", tr("Tau Empire"));
        return;
    }

    if (raceString->contains("ork_race"))
    {
        raceString->replace("ork_race", tr("Orks"));
        return;
    }

    if (raceString->contains("chaos_marine_race"))
    {
        raceString->replace("chaos_marine_race", tr("Chaos"));
        return;
    }

    if (raceString->contains("necron_race"))
    {
        raceString->replace("necron_race", tr("Necrons"));
        return;
    }

    if (raceString->contains("space_marine_race"))
    {
        raceString->replace("space_marine_race", tr("Space Marines"));
        return;
    }

    if (raceString->contains("sisters_race"))
    {
        raceString->replace("sisters_race", tr("Sisters of Battle"));
        return;
    }

    if (raceString->contains("dark_eldar_race"))
    {
        raceString->replace("dark_eldar_race", tr("Dark Eldar"));
        return;
    }

    if (raceString->contains("eldar_race"))
    {
        raceString->replace("eldar_race", tr("Eldar"));
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

    m_player0Race = "";
    m_player1Race = "";
    m_player2Race = "";
    m_player3Race = "";
    m_player4Race = "";
    m_player5Race = "";
    m_player6Race = "";
    m_player7Race = "";

    if (!m_smallPannelActive)
    {
        if(!m_testStats.at(0).name.isEmpty())
            m_player0Race = tr("Team ") + QString::number(m_testStats.at(0).team.toInt() + 1) + ": " + m_testStats.at(0).name + " - " + m_testStats.at(0).race;

        if(!m_testStats.at(1).name.isEmpty())
            m_player1Race = tr("Team ") + QString::number(m_testStats.at(1).team.toInt() + 1) + ": " + m_testStats.at(1).name + " - " + m_testStats.at(1).race;

        if(!m_testStats.at(2).name.isEmpty())
            m_player2Race = tr("Team ") + QString::number(m_testStats.at(2).team.toInt() + 1) + ": " + m_testStats.at(2).name + " - " + m_testStats.at(2).race;

        if(!m_testStats.at(3).name.isEmpty())
            m_player3Race = tr("Team ") + QString::number(m_testStats.at(3).team.toInt() + 1) + ": " + m_testStats.at(3).name + " - " + m_testStats.at(3).race;

        if(!m_testStats.at(4).name.isEmpty())
            m_player4Race = tr("Team ") + QString::number(m_testStats.at(4).team.toInt() + 1) + ": " + m_testStats.at(4).name + " - " + m_testStats.at(4).race;

        if(!m_testStats.at(5).name.isEmpty())
            m_player5Race = tr("Team ") + QString::number(m_testStats.at(5).team.toInt() + 1) + ": " + m_testStats.at(5).name + " - " + m_testStats.at(5).race;

        if(!m_testStats.at(6).name.isEmpty())
            m_player6Race = tr("Team ") + QString::number(m_testStats.at(6).team.toInt() + 1) + ": " + m_testStats.at(6).name + " - " + m_testStats.at(6).race;

        if(!m_testStats.at(7).name.isEmpty())
            m_player7Race = tr("Team ") + QString::number(m_testStats.at(7).team.toInt() + 1) + ": " + m_testStats.at(7).name + " - " + m_testStats.at(7).race;
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

bool GamePanel::rankedModePanelVisible() const
{
    return m_rankedModePanelVisible;
}

void GamePanel::setRankedModePanelVisible(bool newRankedModePanelVisible)
{
    if (m_rankedModePanelVisible == newRankedModePanelVisible)
        return;
    m_rankedModePanelVisible = newRankedModePanelVisible;
    emit rankedModePanelVisibleChanged();
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

void GamePanel::setGamePanelVisible(bool newGamePanelVisible)
{
    m_gamePanelVisible = newGamePanelVisible;
    emit gamePanelVisibleChanged(m_gamePanelVisible);
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

bool GamePanel::gameRankedMode() const
{
    return m_gameRankedMode;
}

void GamePanel::setGameRankedMode(bool newGameRankedMode)
{
    if (m_gameRankedMode == newGameRankedMode)
        return;

    m_gameRankedMode = newGameRankedMode;
    emit gameRankedModeChanged();
}
