#include "replayManager.h"
#include <QDir>
#include <QDebug>
#include <baseTypes.h>
#include <repreader.h>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>

using namespace ReplayReader;

ReplayManager::ReplayManager(QObject *parent)
    : QObject(parent)
    , m_playersListModel( new PlayersListModel(this))
    , m_replaysListModel( new ReplaysListModel(this))

{
    emit replaysListModelSetded();
}

void ReplayManager::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;

    getReplaysData();
}

void ReplayManager::openPlaybackFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_ssPath + QDir::separator() + "Playback"));
}

void ReplayManager::openPlayback(QString fileName)
{
    QDir dir(m_ssPath + QDir::separator() + "Playback");
    QString path = dir.absolutePath();

    QFileInfo replayFile(path + QDir::separator() + fileName);

    if(!replayFile.exists())
        return;

    m_currentReplayTime = replayFile.birthTime().toString("dd.MM.yyyy hh:mm");

    RepReader newRepReader(path + QDir::separator() + fileName);

    m_currentReplayName = newRepReader.replay.Name;
    m_currentFileName = fileName;
    m_currentMod = newRepReader.replay.MOD;
    m_currentModVersion = newRepReader.replay.Version;
    m_currentMap = newRepReader.replay.Map;

    int duration = newRepReader.replay.Duration;

    QString minutes = QString::number(duration / 60);
    QString secs = QString::number(duration - ((duration / 60) * 60));

    if(secs.count() == 1)
        secs = "0" + secs;

    m_currentDuration = minutes + ":" + secs;
    m_currentPlayerCount = newRepReader.replay.PlayerCount;
    m_currentTeamsCount = newRepReader.replay.getTeamsCount();
    m_currentMapSize = newRepReader.replay.MapSize;

    m_mapSourceUrl = "qrc:/maps/resources/maps/" + m_currentMap.toLower() + ".jpg";

    QFile checkFile(m_mapSourceUrl.right(m_mapSourceUrl.count() - 3));

    if(!checkFile.exists())
        m_mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";


    emit updateReplayInfo();


    QVector<ReplayPlayer> replayPlayers;

    QList<Player*> players = newRepReader.replay.Players;

    for(int i = 0; i < players.count(); i++)
    {
        if (players[i]->Type == 7 || players[i]->Type > 11)
            continue;

        ReplayPlayer newPlayer;

        newPlayer.name = players[i]->Name;
        newPlayer.team = QString::number(players[i]->Team);

        switch(players[i]->Type)
        {
            case(0): newPlayer.type = "Player"; break;
            case(1): newPlayer.type = "Computer"; break;
            case(2): newPlayer.type = "Player"; break;
            case(3): newPlayer.type = "Computer"; break;
            case(4): newPlayer.type = "Observer"; break;
            case(11): newPlayer.type = "Computer"; break;
            default: newPlayer.type =  "EmptySlot"; break;
        }

        if (players[i]->isSpectator())
            newPlayer.type = "Observer";

        QString race = players[i]->Race;
        replaceRaceKeyword(&race);
        newPlayer.race = race;

        newPlayer.color = chooseColorForPlayer(players[i]->Team - 1);

        replayPlayers.append(newPlayer);

    }

    m_playersListModel->setPlayersList(std::move(replayPlayers));
}

void ReplayManager::update()
{
    getReplaysData();
}

void ReplayManager::getReplaysData()
{
    QDir dir(m_ssPath + QDir::separator() + "Playback");
    QString path = dir.absolutePath();

    QFileInfoList dirContent = dir.entryInfoList(QStringList() << "*.rec", QDir::Files | QDir::NoDotAndDotDot);

    if (dirContent.count() == 0)
        return;

    QVector<ReplayListInfo> replaysInfo;

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString fileName = dirContent.at(i).fileName();

        RepReader newRepReader(path + QDir::separator() + fileName);

        ReplayListInfo newReplayInfo;

        newReplayInfo.name = newRepReader.replay.Name;

        QString mapSourceUrl = "qrc:/maps/resources/maps/" + newRepReader.replay.Map.toLower() + ".jpg";

        QFile checkFile(m_mapSourceUrl.right(m_mapSourceUrl.count() - 3));

        if(!checkFile.exists())
            m_mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";

        newReplayInfo.map = newRepReader.replay.Map.toUpper();
        newReplayInfo.mapUrl = mapSourceUrl;
        newReplayInfo.mod = newRepReader.replay.MOD;
        newReplayInfo.fileName = fileName;

        QFileInfo replayFile(path + QDir::separator() + fileName);

        if(!replayFile.exists())
            continue;

        newReplayInfo.time = replayFile.birthTime();

        replaysInfo.append(newReplayInfo);

        if (i == 0)
            openPlayback(fileName);
    }

    m_replaysListModel->setReplaysList(std::move(replaysInfo));
}

void ReplayManager::replaceRaceKeyword(QString *raceString)
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

QString ReplayManager::chooseColorForPlayer(uint team)
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
