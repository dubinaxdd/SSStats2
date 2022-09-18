#include "replayManager.h"
#include <QDir>
#include <QDebug>
#include <baseTypes.h>
#include <repreader.h>
#include <QDesktopServices>
#include <QByteArray>
#include <QColor>

#include <QFile>
#include <logger.h>

using namespace ReplayReader;

ReplayManager::ReplayManager(ImageProvider* imageProvider, QObject *parent)
    : QObject(parent)
    , m_playersListModel( new PlayersListModel(this))
    , m_replaysListModel( new ReplaysListModel(this))
    , p_imageProvider(imageProvider)
{
    QObject::connect(m_replaysListModel, &ReplaysListModel::select, this, &ReplayManager::openPlayback, Qt::QueuedConnection);
    emit replaysListModelSetded();
}

void ReplayManager::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;
    m_ssUrlPathPath = QUrl::fromLocalFile(m_ssPath);
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

    m_currentFilePath = QUrl::fromLocalFile(path);
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



    QString winConditions;

    if(newRepReader.replay.hasAnnihilate)
        winConditions += "\nAnnihilate ";

    if(newRepReader.replay.hasTakeAndHold)
        winConditions += "\nTake And Hold ";

    if(newRepReader.replay.hasDestroyHQ)
        winConditions += "\nDestroy HQ ";

    if(newRepReader.replay.hasControlArea)
        winConditions += "\nControl Area ";

    if(newRepReader.replay.hasEconomicVictory)
        winConditions += "\nEconomic Victory ";

    if(newRepReader.replay.hasSuddenDeath)
        winConditions += "\nSudden Death ";

    if(newRepReader.replay.hasAssassinate)
        winConditions += "\nAssassinate ";

    if(newRepReader.replay.hasGameTimer)
        winConditions += "\nGame Timer ";

    winConditions.remove(0, 1);

    setWinConditions(winConditions);


    QString gameSettings;

    gameSettings += newRepReader.replay.settings.getResolvedAIDiff();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedResources();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedLockTeams();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedCheats();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedPositions();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedGameSpeed();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedResourceSharing();
    gameSettings += "\n" + newRepReader.replay.settings.getResolvedResourceSpeed();

    setGameSettings(gameSettings);


    QVector<ReplayPlayer> replayPlayers;

    QList<Player*> players = newRepReader.replay.Players;

    for(int i = 0; i < players.count(); i++)
    {
        if (players[i]->Type == 7 || players[i]->Type > 11)
            continue;

        ReplayPlayer newPlayer;

        newPlayer.name = players[i]->Name;

        if (newPlayer.name.isEmpty())
            continue;

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
        {
            newPlayer.type = "Observer";
            newPlayer.team = "9";
        }

        p_imageProvider->addReplayPlayerBanner("playerBanner" + QString::number(i), players[i]->banner);
        p_imageProvider->addReplayPlayerBanner("playerBadge" + QString::number(i), players[i]->badge);

        newPlayer.bannerUrl = "playerBanner" + QString::number(i);
        newPlayer.badgeUrl = "playerBadge" + QString::number(i);

        QString race = players[i]->Race;
        replaceRaceKeyword(&race);
        newPlayer.race = race;

        newPlayer.color = chooseColorForPlayer(players[i]->Team - 1);

        replayPlayers.append(newPlayer);

    }

    for(int i = 0; i < replayPlayers.count(); i++)
    {
        for(int j = 0; j < replayPlayers.count()-1; j++)
        {
            if (replayPlayers.at(j).team > replayPlayers.at(j+1).team)
            {
                ReplayPlayer buffer = replayPlayers.at(j);
                replayPlayers[j] = replayPlayers[j+1];
                replayPlayers[j+1] = buffer;
            }
        }
    }

    m_playersListModel->setPlayersList(std::move(replayPlayers));
}

void ReplayManager::update()
{
    getReplaysData();
}

void ReplayManager::removeReplay(QString fileName)
{
    QDir dir(m_ssPath + QDir::separator() + "Playback");
    QString path = dir.absolutePath();

    QFile replayFile(path + QDir::separator() + fileName);

    if (replayFile.remove())
        qInfo(logInfo()) << "Removed replay from " + path + QDir::separator() + fileName;
    else
        qWarning(logWarning()) << "Remove replay FAILED from " + path + QDir::separator() + fileName;

    update();
}

void ReplayManager::saveReplay( QString fileName)
{
    QString path = m_currentFilePath.toString().replace("file:///","") + QDir::separator() + m_currentFileName;
    QFile::copy(path, fileName.replace("file:///",""));
}

void ReplayManager::saveBadges(QString filePath, QString imageUrl, int width, int height)
{
    QSize imgSize(width, height);
    QImage savedImage = p_imageProvider->requestImage(imageUrl, &imgSize, imgSize);

    QByteArray imageByteArray;
    imageByteArray.resize(18);

    imageByteArray[0] = 0x00;
    imageByteArray[1] = 0x00;
    imageByteArray[2] = 0x02;
    imageByteArray[3] = 0x00;
    imageByteArray[4] = 0x00;
    imageByteArray[5] = 0x00;
    imageByteArray[6] = 0x00;
    imageByteArray[7] = 0x00;
    imageByteArray[8] = 0x00;
    imageByteArray[9] = 0x00;
    imageByteArray[10] = 0x00;
    imageByteArray[11] = 0x00;
    imageByteArray[12] = static_cast<quint8>(width);
    imageByteArray[13] = 0x00;
    imageByteArray[14] = static_cast<quint8>(height);
    imageByteArray[15] = 0x00;
    imageByteArray[16] = 0x20;
    imageByteArray[17] = 0x08;

    for (int y = height - 1; y >= 0 ; y--)
    {
        for (int x = 0; x < width; x++)
        {
            QColor color = savedImage.pixelColor(x,y);

            imageByteArray.append(static_cast<quint8>(color.blue()));
            imageByteArray.append(static_cast<quint8>(color.green()));
            imageByteArray.append(static_cast<quint8>(color.red()));
            imageByteArray.append(static_cast<quint8>(color.alpha()));
        }
    }

    QFile file(filePath.replace("file:///",""));

    if (!file.open(QFile::WriteOnly))
        return;

    file.write(imageByteArray);
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
      /*  case 0 : return "#b3ea0000";
        case 1 : return "#b30469ee";
        case 2 : return "#b3dacfcf";
        case 3 : return "#b3f6f200";
        case 4 : return "#b30dff1e";
        case 5 : return "#b3f71df4";
        case 6 : return "#b300e6d9";
        case 7 : return "#b3f97dfd";

        */


        case 0 : return "#55ea0000";
        case 1 : return "#550469ee";
        case 2 : return "#55dacfcf";
        case 3 : return "#55f6f200";
        case 4 : return "#550dff1e";
        case 5 : return "#55f71df4";
        case 6 : return "#5500e6d9";
        case 7 : return "#55f97dfd";
    }
    return "#DCDCDC";
}

const QString &ReplayManager::gameSettings() const
{
    return m_gameSettings;
}

void ReplayManager::setGameSettings(const QString &newGameSettings)
{
    if (m_gameSettings == newGameSettings)
        return;
    m_gameSettings = newGameSettings;
    emit gameSettingsChanged();
}

const QString &ReplayManager::winConditions() const
{
    return m_winConditions;
}

void ReplayManager::setWinConditions(const QString &newWinConditions)
{
    if (m_winConditions == newWinConditions)
        return;
    m_winConditions = newWinConditions;
    emit winConditionsChanged();
}
