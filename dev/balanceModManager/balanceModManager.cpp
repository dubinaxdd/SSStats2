#include "balanceModManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include "JlCompress.h"

BalanceModManager::BalanceModManager(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_balanceModInstaller(new BalanceModInstaller())
    , m_balanceModInstallerThread(new QThread(this))
    , m_settingsController(settingsController)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_modsInfoRequestTimer(new QTimer(this))
{
    m_modsInfoRequestTimer->setInterval(5000);
    connect(m_modsInfoRequestTimer, &QTimer::timeout, this, &BalanceModManager::modsInfoTimerTimeout, Qt::QueuedConnection);

    connect(this, &BalanceModManager::installMod, m_balanceModInstaller, &BalanceModInstaller::installMod, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modInstalled, this, &BalanceModManager::onModInstalled, Qt::QueuedConnection);

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

void BalanceModManager::downloadMod(QString modTechnicalName)
{
    m_modDownloadingProcessed = true;

    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/dow_stats_balance_mod/" + modTechnicalName + ".zip";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMod(reply, modTechnicalName);
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

    QString urlString = "https://dowstats.ru/dow_stats_balance_mod/mods.txt";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveVersionsInfo(reply);
    });
}

void BalanceModManager::checkCurrentModInGame()
{
    QSettings* ssSettings = new QSettings(m_ssPath + "\\Local.ini", QSettings::Format::IniFormat);
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
    QFile chanfeLogFile(m_ssPath + QDir::separator() + modTechnicalName + QDir::separator() + modTechnicalName + ".txt");

    if (!chanfeLogFile.open(QFile::ReadOnly))
        return "";

    return QString::fromStdString(chanfeLogFile.readAll().toStdString());
}

void BalanceModManager::newActualModDetected(QString modTechnicalName, bool installed)
{
    m_settingsController->getSettings()->lastActualBalanceMod = modTechnicalName;
    m_settingsController->saveSettings();

    if (!installed && m_settingsController->getSettings()->autoUpdateBalanceMod)
        requestDownloadMod(modTechnicalName);

}

void BalanceModManager::modsInfoTimerTimeout()
{
    checkCurrentModInGame();
    downloadModsInfo();
}

void BalanceModManager::onModInstalled(QString modTechnicalName)
{
    qInfo(logInfo()) <<  modTechnicalName + " installed";

    emit sendModDownloaded(modTechnicalName);
    checkDownloadingQuery();
}

void BalanceModManager::onSettingsLoaded()
{
    m_customHotKeysPath = m_settingsController->getSettings()->balanceModHotKeysPath;
    m_customSchemesPath = m_settingsController->getSettings()->balanceModSchemesPath;
    m_lastActualMod = m_settingsController->getSettings()->lastActualBalanceMod;


    if (m_customHotKeysPath.isEmpty())
    {
        QSettings* ssSettings = new QSettings(m_ssPath + "\\Local.ini", QSettings::Format::IniFormat);
        m_currentProfile = ssSettings->value("global/playerprofile", "").toString();
        delete ssSettings;

        if (m_currentProfile.isEmpty())
            return;

        QChar sepr = QDir::separator();

        QString path = m_ssPath + sepr + "Profiles" + sepr + m_currentProfile + sepr + "dxp2" + sepr + "KEYDEFAULTS.LUA";

        QString schemesPath = m_ssPath + sepr + "Profiles" + sepr + m_currentProfile + sepr + "dxp2" + sepr + "schemes";

        if (QDir(schemesPath).exists())
            m_customSchemesPath = schemesPath;

        QFile hotKeysDefaultFile(path);

        if (hotKeysDefaultFile.exists())
            m_customHotKeysPath = path;
    }

    emit sendCustomHotKeysPath(m_customHotKeysPath);
}

void BalanceModManager::requestChangeLog(QString modTechnicalName)
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/dow_stats_balance_mod/changelogs/" + modTechnicalName + ".txt";

    newRequest.setUrl(QUrl(urlString));
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

void BalanceModManager::uninstalMod(QString modTechnicalName)
{
    QDir modDir(m_ssPath + QDir::separator() + modTechnicalName);
    modDir.removeRecursively();

    QFile moduleFile(m_ssPath + QDir::separator() + modTechnicalName  + ".module");
    moduleFile.remove();
}

void BalanceModManager::receiveCustomHotKeyPath(QString customHotKeysPath)
{
    m_customSchemesPath = customHotKeysPath + QDir::separator() + "schemes";
    m_customHotKeysPath = customHotKeysPath + QDir::separator() + "KEYDEFAULTS.LUA";

    m_settingsController->getSettings()->balanceModSchemesPath = m_customSchemesPath;
    m_settingsController->getSettings()->balanceModHotKeysPath = m_customHotKeysPath;
    m_settingsController->saveSettings();
}

void BalanceModManager::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;

    if(m_ssPath.isEmpty())
        return;

    m_modsInfoRequestTimer->start();
    modsInfoTimerTimeout();
}

void BalanceModManager::checkDownloadingQuery()
{
     m_modDownloadingProcessed = false;

    if(m_downloadingQuery.isEmpty())
        return;

    downloadMod(m_downloadingQuery.last());
    m_downloadingQuery.removeLast();
}

void BalanceModManager::receiveVersionsInfo(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveVersionsInfo - Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

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

    QDir dir1(m_ssPath);
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

        newModInfo.technicalName = newObject.value("technicalName").toString();
        newModInfo.uiName = newObject.value("uiName").toString();
        newModInfo.version = newObject.value("version").toString();
        newModInfo.isActual = newObject.value("isActual").toBool();
        newModInfo.isInstalled = entryList.contains(newModInfo.technicalName) && entryList.contains(newModInfo.technicalName + ".module");
        newModInfo.isCurrentMod = m_currentModName.toLower() == newModInfo.technicalName.toLower();

        if (newModInfo.isInstalled)
            newModInfo.changelog = getChangeLogFromLocalFiles(newModInfo.technicalName);

        if (newModInfo.isActual && newModInfo.technicalName != m_lastActualMod)
        {
            newActualModDetected(newModInfo.technicalName, newModInfo.isInstalled);

            if (!newModInfo.isInstalled && m_settingsController->getSettings()->autoUpdateBalanceMod)
                newModInfo.downloadingProcessed = true;
        }

        m_modInfoList.append(newModInfo);
    }

    emit sendModsInfo(m_modInfoList);

    if (m_modInfoList.count() > 0 && !m_modInfoList.first().isInstalled)
        requestChangeLog(m_modInfoList.first().technicalName);
}

void BalanceModManager::receiveMod(QNetworkReply *reply, QString modTechnicalName)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveMod: Connection error:" << reply->errorString();
        delete reply;
        checkDownloadingQuery();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    QString path = QDir::currentPath() + QDir::separator() + "modTechnicalName.zip";

    InstallModData data;

    data.modByteArray = replyByteArray;
    data.filePath = path;
    data.decompressPath = m_ssPath + QDir::separator();
    data.modTechnicalName = modTechnicalName;
    data.hotKeysPath = m_customHotKeysPath;
    data.ssPath = m_ssPath;
    data.schemesPath = m_customSchemesPath;

    emit installMod(data);
}

void BalanceModManager::receiveChangeLog(QNetworkReply *reply, QString modTechnicalName)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "BalanceModManager::receiveChangeLog: Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    emit changeLogReceived(modTechnicalName, QString::fromStdString(replyByteArray.toStdString()));

    delete reply;
}
