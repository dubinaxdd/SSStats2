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
    , m_replaysListModel( new ReplaysListModel(this))
    , m_playersListModel( new PlayersListModel(this))
    , p_imageProvider(imageProvider)
    , m_asyncReplayReader(new AsyncReplayReader())
{
    QObject::connect(m_replaysListModel, &ReplaysListModel::select, this, &ReplayManager::openPlayback, Qt::QueuedConnection);
    QObject::connect(m_asyncReplayReader, &AsyncReplayReader::sendReplaysInfo, this, &ReplayManager::receiveReplaysInfo, Qt::QueuedConnection);
    QObject::connect(this, &ReplayManager::requestReplaysInfo, m_asyncReplayReader, &AsyncReplayReader::readReplaysList, Qt::QueuedConnection);

    emit replaysListModelSetded();

    m_asyncReplayReader->moveToThread(&m_asyncReplayReaderThread);
    m_asyncReplayReaderThread.start();
}

ReplayManager::~ReplayManager()
{
    m_asyncReplayReader->abort();
    m_asyncReplayReaderThread.quit();
    m_asyncReplayReaderThread.wait();
    m_asyncReplayReader->deleteLater();
}

void ReplayManager::setGamePath(GamePath* currentGame)
{
    m_currentGame = currentGame;
    m_ssUrlPathPath = QUrl::fromLocalFile(m_currentGame->gameSettingsPath);
    m_playbackFolder = m_currentGame->gameSettingsPath + QDir::separator() + "Playback";

    qInfo(logInfo()) << "Default playback folder: " << m_playbackFolder;

    getReplaysData();
}

void ReplayManager::openPlaybackFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_currentGame->gameSettingsPath + QDir::separator() + "Playback"));
}

void ReplayManager::openPlayback(QString fileName)
{
    QDir dir(m_playbackFolder);
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

    if (m_currentMod == "dxp2")
        m_currentMod = tr("Original Soulstorm");

    for (int i = 0; i < m_modInfo.count(); i++)
    {
        if (m_modInfo.at(i).technicalName.toLower() == m_currentMod.toLower())
        {
            m_currentMod = m_modInfo.at(i).uiName;
            break;
        }
    }

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
        winConditions += "\n" + tr("Annihilate");

    if(newRepReader.replay.hasTakeAndHold)
        winConditions += "\n" + tr("Take And Hold");

    if(newRepReader.replay.hasDestroyHQ)
        winConditions += "\n" + tr("Destroy HQ");

    if(newRepReader.replay.hasControlArea)
        winConditions += "\n" + tr("Control Area");

    if(newRepReader.replay.hasEconomicVictory)
        winConditions += "\n" + tr("Economic Victory");

    if(newRepReader.replay.hasSuddenDeath)
        winConditions += "\n" + tr("Sudden Death");

    if(newRepReader.replay.hasAssassinate)
        winConditions += "\n" + tr("Assassinate");

    if(newRepReader.replay.hasGameTimer)
        winConditions += "\n" + tr("Game Timer");

    winConditions.remove(0, 1);

    setWinConditions(winConditions);


    QString gameSettings;

    gameSettings += tr(newRepReader.replay.settings.getResolvedAIDiff().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedResources().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedLockTeams().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedCheats().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedPositions().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedGameSpeed().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedResourceSharing().toLatin1());
    gameSettings += "\n" + tr(newRepReader.replay.settings.getResolvedResourceSpeed().toLatin1());

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
            case(0): newPlayer.type = tr("Player"); break;
            case(1): newPlayer.type = tr("Computer"); break;
            case(2): newPlayer.type = tr("Player"); break;
            case(3): newPlayer.type = tr("Computer"); break;
            case(4): newPlayer.type = tr("Observer"); break;
            case(11): newPlayer.type = tr("Computer"); break;
            default: newPlayer.type =  tr("EmptySlot"); break;
        }

        if (players[i]->isSpectator())
        {
            newPlayer.type = tr("Observer");
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

void ReplayManager::receiveModsInfo(QList<ModInfo> modInfo)
{
    m_modInfo = modInfo;
    m_replaysListModel->setModInfo(&m_modInfo);
    m_asyncReplayReader->setModInfo(&m_modInfo);
    emit updateReplayInfo();
}

void ReplayManager::onGamePathChanged()
{
    m_ssUrlPathPath = QUrl::fromLocalFile(m_currentGame->gameSettingsPath);
    m_playbackFolder = m_currentGame->gameSettingsPath + QDir::separator() + "Playback";
    qInfo(logInfo()) << "Default playback folder: " << m_playbackFolder;
    getReplaysData();
}

void ReplayManager::update()
{
    getReplaysData();
}

void ReplayManager::removeReplay(QString fileName)
{
    QDir dir(m_playbackFolder);
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

    if(savedImage.width() > 256 || savedImage.height() > 256)
    {
        qWarning(logWarning()) << "Image size is too large, image not saved";
        return;
    }

    quint16 imgWidth = savedImage.width();
    quint16 imgHeight = savedImage.height();

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
    //imageByteArray[12] = static_cast<quint8>(width);
    imageByteArray[12] = static_cast<quint8>(imgWidth);
    imageByteArray[13] = 0x00;
    //imageByteArray[14] = static_cast<quint8>(height);
    imageByteArray[14] = static_cast<quint8>(imgHeight);
    imageByteArray[15] = 0x00;
    imageByteArray[16] = 0x20;
    imageByteArray[17] = 0x08;

    for (int y = /*height*/imgHeight - 1; y >= 0 ; y--)
    {
        for (int x = 0; x < /*width*/imgWidth; x++)
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

    file.close();
}

void ReplayManager::choiseOtherPlaybackFolder(QString folder)
{
    m_playbackFolder = folder.replace("file:///","");
    getReplaysData();
}

void ReplayManager::choiseDefaultPlaybackFolder()
{
    m_playbackFolder = m_currentGame->gameSettingsPath + QDir::separator() + "Playback";
    getReplaysData();
}

void ReplayManager::findReplays(QString findText)
{
    resetView();
    emit requestReplaysInfo(m_playbackFolder, findText);
}

void ReplayManager::getReplaysData()
{
    resetView();
    emit requestReplaysInfo(m_playbackFolder, "");
}


void ReplayManager::receiveReplaysInfo(QVector<ReplayListInfo> newReplaysList)
{
    m_replaysListModel->setReplaysList(std::move(newReplaysList));
}

void ReplayManager::replaceRaceKeyword(QString *raceString)
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

QString ReplayManager::chooseColorForPlayer(uint team)
{
    switch (team)
    {
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

void ReplayManager::resetView()
{
    QVector<ReplayListInfo> newReplaysList;
    m_replaysListModel->setReplaysList(std::move(newReplaysList));

    m_currentReplayTime = "";
    m_currentFilePath = "";
    m_currentReplayName = "";
    m_currentFileName = "";
    m_currentMod = "";
    m_currentModVersion = "";
    m_currentMap = "";
    m_currentDuration = "";
    m_currentPlayerCount = 0;
    m_currentTeamsCount = 0;
    m_currentMapSize = 0;

    setWinConditions("");
    setGameSettings("");

    QVector<ReplayPlayer> replayPlayers;
    m_playersListModel->setPlayersList(std::move(replayPlayers));

    m_mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";
    emit updateReplayInfo();
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
