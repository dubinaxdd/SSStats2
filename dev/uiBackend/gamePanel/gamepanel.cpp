#include "gamepanel.h"
#include <QDebug>

GamePanel::GamePanel(QObject *parent) : QObject(parent)
{
    m_racePanelVisibleTimer = new QTimer(this);
    m_racePanelVisibleTimer->setInterval(20000);
    m_racePanelVisibleTimer->setSingleShot(true);

    QObject::connect(m_racePanelVisibleTimer, &QTimer::timeout, this, &GamePanel::racePanelVisibleTimerTimeout, Qt::QueuedConnection);
}

void GamePanel::racePanelVisibleTimerTimeout()
{
    m_racePanelVisisble = false;
    emit racePanelVisibleChanged(m_racePanelVisisble);
}

void GamePanel::onCurrentApmChanged(quint64 val)
{
    m_currentApm = QString::number(val);
    emit currentApmUpdate();
}

void GamePanel::onAverageApmChanged(quint64 val)
{
    m_averageApm = QString::number(val);
    emit averageApmUpdate();
}

void GamePanel::onGameStopped()
{
    m_currentApm = QString("-");
    m_averageApm = QString("-");
    emit currentApmUpdate();
    emit averageApmUpdate();

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


void GamePanel::receivePlayersTestStats(QVector<PlayerStats> testStats)
{
    m_racePanelVisibleTimer->start();

    m_gamePanelVisisble = true;
    emit gamePanelVisibleChanged(m_gamePanelVisisble);

    m_racePanelVisisble = true;
    emit racePanelVisibleChanged(m_racePanelVisisble);

    if(testStats.count() < 8)
        return;

    if(!testStats.at(0).name.isEmpty())
        m_player0Race = "Team " + QString::number(testStats.at(0).team.toInt() + 1) + ": " + testStats.at(0).name/*.toLocal8Bit()*/ + " - " + testStats.at(0).race;

    if(!testStats.at(1).name.isEmpty())
        m_player1Race = "Team " + QString::number(testStats.at(1).team.toInt() + 1) + ": " + testStats.at(1).name/*.toLocal8Bit()*/  + " - " + testStats.at(1).race;

    if(!testStats.at(2).name.isEmpty())
        m_player2Race = "Team " + QString::number(testStats.at(2).team.toInt() + 1) + ": " + testStats.at(2).name/*.toLocal8Bit()*/  + " - " + testStats.at(2).race;

    if(!testStats.at(3).name.isEmpty())
        m_player3Race = "Team " + QString::number(testStats.at(3).team.toInt() + 1) + ": " + testStats.at(3).name/*.toLocal8Bit()*/  + " - " + testStats.at(3).race;

    if(!testStats.at(4).name.isEmpty())
        m_player4Race = "Team " + QString::number(testStats.at(4).team.toInt() + 1) + ": " + testStats.at(4).name/*.toLocal8Bit()*/  + " - " + testStats.at(4).race;

    if(!testStats.at(5).name.isEmpty())
        m_player5Race = "Team " + QString::number(testStats.at(5).team.toInt() + 1) + ": " + testStats.at(5).name/*.toLocal8Bit()*/  + " - " + testStats.at(5).race;

    if(!testStats.at(6).name.isEmpty())
        m_player6Race = "Team " + QString::number(testStats.at(6).team.toInt() + 1) + ": " + testStats.at(6).name/*.toLocal8Bit()*/  + " - " + testStats.at(6).race;

    if(!testStats.at(7).name.isEmpty())
        m_player7Race = "Team " + QString::number(testStats.at(7).team.toInt() + 1) + ": " + testStats.at(7).name/*.toLocal8Bit()*/  + " - " + testStats.at(7).race;


    replaceRaceKeyword(&m_player0Race);
    replaceRaceKeyword(&m_player1Race);
    replaceRaceKeyword(&m_player2Race);
    replaceRaceKeyword(&m_player3Race);
    replaceRaceKeyword(&m_player4Race);
    replaceRaceKeyword(&m_player5Race);
    replaceRaceKeyword(&m_player6Race);
    replaceRaceKeyword(&m_player7Race);

    m_player0Color = chooseColorForPlayer(testStats.at(0).team.toInt());
    m_player1Color = chooseColorForPlayer(testStats.at(1).team.toInt());
    m_player2Color = chooseColorForPlayer(testStats.at(2).team.toInt());
    m_player3Color = chooseColorForPlayer(testStats.at(3).team.toInt());
    m_player4Color = chooseColorForPlayer(testStats.at(4).team.toInt());
    m_player5Color = chooseColorForPlayer(testStats.at(5).team.toInt());
    m_player6Color = chooseColorForPlayer(testStats.at(6).team.toInt());
    m_player7Color = chooseColorForPlayer(testStats.at(7).team.toInt());

    emit playerTestStatsUpdate();
}

void GamePanel::expandPlayerRacesButtonClick()
{
    m_racePanelVisisble = !m_racePanelVisisble;
    emit racePanelVisibleChanged(m_racePanelVisisble);
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

void GamePanel::setGamePanelVisisble(bool newGamePanelVisisble)
{
    m_gamePanelVisisble = newGamePanelVisisble;
    emit gamePanelVisibleChanged(m_gamePanelVisisble);
}

