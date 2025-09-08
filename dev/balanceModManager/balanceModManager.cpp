#include "balanceModManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include "JlCompress.h"
#include "defines.h"

BalanceModManager::BalanceModManager(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_balanceModInstaller(new BalanceModInstaller())
    , m_balanceModInstallerThread(new QThread(this))
    , m_checkDownloadingQueryTimer(new QTimer(this))
    , m_settingsController(settingsController)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_modsInfoRequestTimer(new QTimer(this))
{
    m_modsInfoRequestTimer->setInterval(5000);
    connect(m_modsInfoRequestTimer, &QTimer::timeout, this, &BalanceModManager::modsInfoTimerTimeout, Qt::QueuedConnection);

    m_checkDownloadingQueryTimer->setInterval(1000);
    connect(m_checkDownloadingQueryTimer, &QTimer::timeout, this, &BalanceModManager::checkDownloadingQuery, Qt::QueuedConnection);

    connect(this, &BalanceModManager::installMod, m_balanceModInstaller, &BalanceModInstaller::installMod, Qt::QueuedConnection);
    connect(this, &BalanceModManager::uninstallMod, m_balanceModInstaller, &BalanceModInstaller::uninstallMod, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modInstalled, this, &BalanceModManager::onModInstalled, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modUninstalled, this, &BalanceModManager::onModUninstalled, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modInstallError, this, [=](QString modTechnicalName){emit sendInstallingModError(modTechnicalName); }, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::hotKeysUpdated, this,  [=](QString modTechnicalName, bool result){emit onHotKeysUpdated(modTechnicalName, result);}, Qt::QueuedConnection);


    connect(this, &BalanceModManager::sendUpdateHotKeysOnMod, m_balanceModInstaller, &BalanceModInstaller::updateHotKeysOnMod, Qt::QueuedConnection);


    connect(m_settingsController, &SettingsController::settingsLoaded, this, &BalanceModManager::onSettingsLoaded, Qt::QueuedConnection);

    m_balanceModInstaller->moveToThread(m_balanceModInstallerThread);
    m_balanceModInstallerThread->start();
}

BalanceModManager::~BalanceModManager()
{
    m_balanceModInstallerThread->quit();
    m_balanceModInstallerThread->wait();
    m_balanceModInstaller->deleteLater();
}

void BalanceModManager::downloadMod(QString modTechnicalName, bool overwritePrifiles)
{
    m_modDownloadingProcessed = true;

    QNetworkRequest newRequest;

    QString urlString = m_balanceModServerAddres + modTechnicalName + ".zip";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("X-Key", BALANCE_MOD_KEY);

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMod(reply, modTechnicalName, overwritePrifiles);
    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){

        if(bytesTotal != 0)
        {
            emit sendModDownloadProgress(bytesReceived * 100 / bytesTotal, modTechnicalName);
            qInfo(logInfo()) << modTechnicalName + " download progress:" << bytesReceived << "/" << bytesTotal;
        }
    });
}

void BalanceModManager::downloadModsInfo()
{
    QNetworkRequest newRequest;

    QString urlString = m_balanceModServerAddres + "mods.txt";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("X-Key", BALANCE_MOD_KEY);

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveVersionsInfo(reply);
    });
}

void BalanceModManager::checkCurrentModInGame()
{
    QDir ssPath(m_currentGame->gameSettingsPath);

    if(!ssPath.exists())
        return;

    QSettings* ssSettings = new QSettings(m_currentGame->gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
    QString currentModName = ssSettings->value("global/currentmoddc", "").toString();
    delete ssSettings;

    if (currentModName != m_currentModName)
    {
        m_currentModName = currentModName;
        emit sendCurrentModInGame(m_currentModName);
    }
}

QString BalanceModManager::getChangeLogFromLocalFiles(QString modTechnicalName)
{
    QFile chanfeLogFile(m_currentGame->gamePath + QDir::separator() + modTechnicalName + QDir::separator() + modTechnicalName + ".txt");

    if (!chanfeLogFile.open(QFile::ReadOnly))
        return "";

    return QString::fromStdString(chanfeLogFile.readAll().toStdString());
}

bool BalanceModManager::getProfileExist(QString modTechnicalName)
{

    QDir dir(m_currentGame->gameSettingsPath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirContent.count(); i++)
    {
        if (QDir().exists(dirContent.at(i).absoluteFilePath() + QDir::separator() + modTechnicalName.toLower()))
            return true;
    }

    return false;
}

void BalanceModManager::newActualModDetected(QString modTechnicalName, bool installed)
{
    if (!installed)
        return;

    m_settingsController->getSettings()->lastActualBalanceMod = modTechnicalName;
    m_settingsController->saveSettings();

    m_lastActualMod = m_settingsController->getSettings()->lastActualBalanceMod;
}

void BalanceModManager::modsInfoTimerTimeout()
{
    if(!m_betaTestPlayersListReceived)
    {
        requestBetaTestPlayersList();
        return;
    }

    if(m_currentGame->gamePath.isEmpty() || !m_gameLounchedStateReceived)
        return;

    checkCurrentModInGame();
    downloadModsInfo();
}

void BalanceModManager::onModInstalled(QString modTechnicalName)
{
    for(int i = 0; i < m_modInfoList.count(); i++)
    {
        if (m_modInfoList.at(i).technicalName == modTechnicalName)
        {
            m_modInfoList[i].isInstalled = true;
            updateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);

            if (m_modInfoList.at(i).isLatest)
                newActualModDetected(modTechnicalName, true);

            break;
        }
    }

    qInfo(logInfo()) <<  modTechnicalName + " installed";
    emit sendModDownloaded(modTechnicalName);

    m_modDownloadingProcessed = false;
    checkDownloadingQuery();


}

void BalanceModManager::onModUninstalled(QString modTechnicalName)
{
    for(int i = 0; i < m_modInfoList.count(); i++)
    {
        if (m_modInfoList.at(i).technicalName == modTechnicalName)
        {
            m_modInfoList[i].isInstalled = false;
            updateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);
            break;
        }
    }
}

void BalanceModManager::onSettingsLoaded()
{
    qInfo(logInfo()) << "BalanceModManager::onSettingsLoaded()" << "load started";

    m_templateProfilePath = m_settingsController->getSettings()->templateProfilePath;
    m_lastActualMod = m_settingsController->getSettings()->lastActualBalanceMod;

    if(QDir(m_templateProfilePath).exists())
        emit sendTemplateProfilePath(m_templateProfilePath);

    qInfo(logInfo()) << "BalanceModManager::onSettingsLoaded()" << "load finished";
}

void BalanceModManager::requestChangeLog(QString modTechnicalName)
{
    QNetworkRequest newRequest;

    QString urlString = m_balanceModServerAddres + "changelogs/" + modTechnicalName + ".txt";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("X-Key", BALANCE_MOD_KEY);

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveChangeLog(reply, modTechnicalName);
    });
}

void BalanceModManager::requestDownloadMod(QString modTechnicalName)
{
    m_downloadingQuery.append(modTechnicalName);

    if (!m_modDownloadingProcessed)
        checkDownloadingQuery();
}

void BalanceModManager::requestBetaTestPlayersList()
{
    QNetworkRequest newRequest;

    QString urlString = m_balanceModServerAddres + "beta_test_players_list.txt";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("X-Key", BALANCE_MOD_KEY);

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveBetaTestPlayersList(reply);
    });
}

void BalanceModManager::receiveBetaTestPlayersList(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveBetaTestPlayersList - Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "BalanceModManager::receiveBetaTestPlayersList: beta_test_players_list.txt parsing error: " + replyByteArray;
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    for (int i = 0; i < jsonArray.count(); i++)
    {
        if(!jsonArray.at(i).isObject())
        {
            qWarning(logWarning()) << "BalanceModManager::receiveBetaTestPlayersList: Value at " << i << "is not a JsonObject" + replyByteArray;
            continue;
        }

        QJsonObject newObject = jsonArray.at(i).toObject();

        QString steamId = newObject.value("steamId").toString();

        if (steamId == m_currentPlayerSteamId)
        {
            m_showBalanceModBetaVersions = true;
            qInfo(logInfo()) << "Beta versions of balance mod is avilable.";
            emit showBalanceModBetaVersionsChanged(m_showBalanceModBetaVersions);
            break;
        }
    }

    if (!m_showBalanceModBetaVersions)
        qInfo(logInfo()) << "Beta versions of balance mod is not avilable.";

    m_betaTestPlayersListReceived = true;

    modsInfoTimerTimeout();
}

void BalanceModManager::uninstalMod(QString modTechnicalName)
{
    emit uninstallMod(m_currentGame->gamePath, modTechnicalName);
}

void BalanceModManager::receiveTemplateProfilePath(QString templateProfilePath)
{
    m_templateProfilePath = templateProfilePath;

    m_settingsController->getSettings()->templateProfilePath = m_templateProfilePath;
    m_settingsController->saveSettings();
}

void BalanceModManager::activateMod(QString modTechnicalName)
{
    QSettings* ssSettings = new QSettings(m_currentGame->gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
    ssSettings->setValue("global/currentmoddc", modTechnicalName.toLower());
    delete ssSettings;
}

void BalanceModManager::updateTemplateProfilePath(bool useCustomTemplateProfilePath)
{
    if (!useCustomTemplateProfilePath)
    {
        QString technicalName = "";

        for(int i = 0; i < m_modInfoList.count(); i++)
        {
            if (m_modInfoList.at(i).isInstalled)
            {
                technicalName = m_modInfoList.at(i).technicalName;
                break;
            }
        }

        if (!technicalName.isEmpty())
            updateTemplateProfilePath(technicalName);
        else
            updateTemplateProfilePath("dxp2");
    }
}

void BalanceModManager::setProfileCopyMode(bool overwritePrifiles, QString modTechnicalName)
{
    m_downloadingQuery.removeOne(modTechnicalName);
    downloadMod(modTechnicalName, overwritePrifiles);
}

void BalanceModManager::onGameLaunchStateChanged(bool lounched)
{
    m_gameLounchedState = lounched;
    m_gameLounchedStateReceived = true;

    if (m_currentPlayerSteamId.isEmpty())
        return;

    modsInfoTimerTimeout();
}

void BalanceModManager::setCurrentPlayerSteamId(QString steamId)
{
    m_currentPlayerSteamId = steamId;

    m_modsInfoRequestTimer->start();
    m_checkDownloadingQueryTimer->start();
}

void BalanceModManager::updateHotKeysOnMod(QString modTechnicalName)
{
    emit sendUpdateHotKeysOnMod(modTechnicalName, m_currentGame->gameSettingsPath);
}

void BalanceModManager::onCurrentGameChanged()
{
    m_modInfoHash = "";
    downloadModsInfo();
}

bool BalanceModManager::showBalanceModBetaVersions() const
{
    return m_showBalanceModBetaVersions;
}

void BalanceModManager::updateTemplateProfilePath(QString modTechnicalName)
{
    QChar sepr = QDir::separator();

    QSettings* ssSettings = new QSettings(m_currentGame->gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile", "").toString();
    delete ssSettings;

    if (m_currentProfile.isEmpty())
        return;

    QString path = m_currentGame->gameSettingsPath + sepr + "Profiles" + sepr + m_currentProfile + sepr + modTechnicalName.toLower();

    if (QDir(path).exists())
        m_templateProfilePath = path;

    if (QDir(m_templateProfilePath).exists())
    {
        emit sendTemplateProfilePath(m_templateProfilePath);
        m_settingsController->getSettings()->templateProfilePath = m_templateProfilePath;
        m_settingsController->saveSettings();
    }

}

void BalanceModManager::setGamePath(GamePath* currentGame)
{
    m_currentGame = currentGame;

    QDir gamePath(currentGame->gamePath);

    if(!gamePath.exists())
        return;

    if (m_currentPlayerSteamId.isEmpty())
        return;

    modsInfoTimerTimeout();
}

void BalanceModManager::checkDownloadingQuery()
{
    if(m_downloadingQuery.isEmpty() || m_gameLounchedState || m_modDownloadingProcessed || m_currentPlayerSteamId == "")
        return;

    if (getProfileExist(m_downloadingQuery.last()))
    {
        m_modDownloadingProcessed = true;
        emit requestProfileCopyMode(m_downloadingQuery.last());
    }
    else
    {
        downloadMod(m_downloadingQuery.last(), true);
        m_downloadingQuery.removeLast();
    }
}

void BalanceModManager::receiveVersionsInfo(QNetworkReply *reply)
{

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveVersionsInfo - Connection error:" << reply->errorString();

        if (m_templateProfilePath.isEmpty())
            updateTemplateProfilePath("dxp2");

        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QCryptographicHash hashGenerator(QCryptographicHash::Sha256);
    QByteArray newHash = hashGenerator.hash(replyByteArray, QCryptographicHash::Sha256);

    if(newHash == m_modInfoHash)
        return;

    m_modInfoHash = newHash;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "BalanceModManager::receiveVersionsInfo: mods.txt parsing error: " + replyByteArray;
        return;
    }

    m_modInfoList.clear();

    QJsonArray jsonArray = jsonDoc.array();

    QDir dir1(m_currentGame->gamePath);
    QStringList entryList = dir1.entryList();

    for (int i = 0; i < jsonArray.count(); i++)
    {
        if(!jsonArray.at(i).isObject())
        {
            qWarning(logWarning()) << "BalanceModManager::receiveVersionsInfo: Value at " << i << "is not a JsonObject" + replyByteArray;
            continue;
        }

        QJsonObject newObject = jsonArray.at(i).toObject();

        ModInfo newModInfo;

        newModInfo.isBeta  = newObject.value("isBeta").toBool();

        if (!m_showBalanceModBetaVersions && newModInfo.isBeta)
            continue;

        newModInfo.technicalName = newObject.value("technicalName").toString();
        newModInfo.uiName = newObject.value("uiName").toString();
        newModInfo.version = newObject.value("version").toString();
        newModInfo.isInstalled = entryList.contains(newModInfo.technicalName) && entryList.contains(newModInfo.technicalName + ".module");
        newModInfo.isCurrentMod = m_currentModName.toLower() == newModInfo.technicalName.toLower();

        if (newModInfo.isInstalled)
            newModInfo.changelog = getChangeLogFromLocalFiles(newModInfo.technicalName).replace('\t', "    ");

        m_modInfoList.append(newModInfo);
    }

    if (m_templateProfilePath.isEmpty())
        updateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);

    bool actualModFinded = false;
    bool previousModFinded = false;

    for (int i = 0; i < m_modInfoList.count(); i++)
    {
        //Определяем предыдущую версию мода
        if(!previousModFinded && actualModFinded && m_modInfoList.at(i).isInstalled)
        {
            m_modInfoList[i].isPrevious = true;
            previousModFinded = true;
        }

        //Определяем актуальную версию мода
        if(!actualModFinded && !m_modInfoList.at(i).isBeta)
        {
            m_modInfoList[i].isLatest = true;
            actualModFinded = true;
        }

        if (m_modInfoList.at(i).isLatest && m_modInfoList.at(i).technicalName != m_lastActualMod)
        {
            newActualModDetected(m_modInfoList.at(i).technicalName, m_modInfoList.at(i).isInstalled);

            if (!m_modInfoList.at(i).isInstalled && m_settingsController->getSettings()->autoUpdateBalanceMod)
            {
                m_modInfoList[i].downloadingProcessed = true;
                requestDownloadMod(m_modInfoList.at(i).technicalName);
            }

            if (!m_modInfoList.at(i).isInstalled && (!m_settingsController->getSettings()->autoUpdateBalanceMod || m_gameLounchedState))
                emit sendModReadyForInstall(m_modInfoList.at(i).uiName);
        }
    }

    emit sendModsInfo(m_modInfoList);

    if (m_modInfoList.count() > 0 && !m_modInfoList.first().isInstalled)
        requestChangeLog(m_modInfoList.first().technicalName);

    emit sendCurrentModInGame(m_currentModName);
}

void BalanceModManager::receiveMod(QNetworkReply *reply, QString modTechnicalName, bool overwritePrifiles)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveMod: Connection error:" << reply->errorString();
        reply->deleteLater();
        emit sendInstallingModError(modTechnicalName);
        m_modDownloadingProcessed = false;
        checkDownloadingQuery();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    QString path = QDir::currentPath() + QDir::separator() + "modTechnicalName.zip";

    InstallModData data;

    data.modByteArray = replyByteArray;
    data.filePath = path;
    data.decompressPath = m_currentGame->gamePath + QDir::separator();
    data.modTechnicalName = modTechnicalName;
    data.templateProfilePath = m_templateProfilePath;
    data.gamePath = m_currentGame->gamePath;
    data.profilePath = m_currentGame->gameSettingsPath;
    data.gameType = m_currentGame->gameType;

    emit installMod(data, overwritePrifiles);
}

void BalanceModManager::receiveChangeLog(QNetworkReply *reply, QString modTechnicalName)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveChangeLog: Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    emit changeLogReceived(modTechnicalName, QString::fromStdString(replyByteArray.toStdString()).replace("\t","    "));

    reply->deleteLater();
}
