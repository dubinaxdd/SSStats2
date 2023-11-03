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
    , m_checkDownloadingQueryTimer(new QTimer(this))
    , m_settingsController(settingsController)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_modsInfoRequestTimer(new QTimer(this))
{
    m_modsInfoRequestTimer->setInterval(5000);
    connect(m_modsInfoRequestTimer, &QTimer::timeout, this, &BalanceModManager::modsInfoTimerTimeout, Qt::QueuedConnection);

    m_modsInfoRequestTimer->start();

    m_checkDownloadingQueryTimer->setInterval(1000);
    connect(m_checkDownloadingQueryTimer, &QTimer::timeout, this, &BalanceModManager::checkDownloadingQuery, Qt::QueuedConnection);

    m_checkDownloadingQueryTimer->start();

    connect(this, &BalanceModManager::installMod, m_balanceModInstaller, &BalanceModInstaller::installMod, Qt::QueuedConnection);
    connect(this, &BalanceModManager::uninstallMod, m_balanceModInstaller, &BalanceModInstaller::uninstallMod, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modInstalled, this, &BalanceModManager::onModInstalled, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modUninstalled, this, &BalanceModManager::onModUninstalled, Qt::QueuedConnection);
    connect(m_balanceModInstaller, &BalanceModInstaller::modInstallError, this, [=](QString modTechnicalName){emit sendInstallingModError(modTechnicalName); }, Qt::QueuedConnection);

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

    QString urlString = "http://crosspick.ru/dow_stats_client/dow_stats_balance_mod/" + modTechnicalName + ".zip";

    newRequest.setUrl(QUrl(urlString));
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

    QString urlString = "http://crosspick.ru/dow_stats_client/dow_stats_balance_mod/mods.txt";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveVersionsInfo(reply);
    });
}

void BalanceModManager::checkCurrentModInGame()
{
    QDir ssPath(m_ssPath);

    if(!ssPath.exists())
        return;

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

bool BalanceModManager::getProfileExist(QString modTechnicalName)
{

    QDir dir(m_ssPath + "\\Profiles\\");
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
    m_settingsController->getSettings()->lastActualBalanceMod = modTechnicalName;
    m_settingsController->saveSettings();

    m_lastActualMod = m_settingsController->getSettings()->lastActualBalanceMod;

    //if (!installed && m_settingsController->getSettings()->autoUpdateBalanceMod)
    //    requestDownloadMod(modTechnicalName);
}

void BalanceModManager::modsInfoTimerTimeout()
{
    if(m_ssPath.isEmpty() || !m_ssLounchedStateReceived)
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
            receiveUpdateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);
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
            receiveUpdateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);
            break;
        }
    }
}

void BalanceModManager::onSettingsLoaded()
{
    m_templateProfilePath = m_settingsController->getSettings()->templateProfilePath;
    m_lastActualMod = m_settingsController->getSettings()->lastActualBalanceMod;

    if(QDir(m_templateProfilePath).exists())
        emit sendTemplateProfilePath(m_templateProfilePath);
}

void BalanceModManager::requestChangeLog(QString modTechnicalName)
{
    QNetworkRequest newRequest;

    QString urlString = "http://crosspick.ru/dow_stats_client/dow_stats_balance_mod/changelogs/" + modTechnicalName + ".txt";

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
    emit uninstallMod(m_ssPath, modTechnicalName);
}

void BalanceModManager::receiveTemplateProfilePath(QString templateProfilePath)
{
    m_templateProfilePath = templateProfilePath;

    m_settingsController->getSettings()->templateProfilePath = m_templateProfilePath;
    m_settingsController->saveSettings();
}

void BalanceModManager::activateMod(QString modTechnicalName)
{
    QSettings* ssSettings = new QSettings(m_ssPath + "\\Local.ini", QSettings::Format::IniFormat);
    ssSettings->setValue("global/currentmoddc", modTechnicalName.toLower());
    delete ssSettings;
}

void BalanceModManager::receiveUpdateTemplateProfilePath(bool useCustomTemplateProfilePath)
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

void BalanceModManager::receiveProfileCopyMode(bool overwritePrifiles, QString modTechnicalName)
{
    m_downloadingQuery.removeOne(modTechnicalName);
    downloadMod(modTechnicalName, overwritePrifiles);
}

void BalanceModManager::onSsLaunchStateChanged(bool lounched)
{
    m_ssLounchedState = lounched;
    m_ssLounchedStateReceived = true;
    qDebug() << "ASDADASDASDASDADADASDASDASDASD" << m_ssLounchedState;

    modsInfoTimerTimeout();

}

void BalanceModManager::updateTemplateProfilePath(QString modTechnicalName)
{
    QChar sepr = QDir::separator();

    QSettings* ssSettings = new QSettings(m_ssPath + "\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile", "").toString();
    delete ssSettings;

    if (m_currentProfile.isEmpty())
        return;

    QString path = m_ssPath + sepr + "Profiles" + sepr + m_currentProfile + sepr + modTechnicalName.toLower();

    if (QDir(path).exists())
        m_templateProfilePath = path;

    if (QDir(m_templateProfilePath).exists())
    {
        emit sendTemplateProfilePath(m_templateProfilePath);
        m_settingsController->getSettings()->templateProfilePath = m_templateProfilePath;
        m_settingsController->saveSettings();
    }

}

void BalanceModManager::setSsPath(const QString &newSsPath)
{
    QDir ssPath(newSsPath);

    if(!ssPath.exists())
        return;

    m_ssPath = newSsPath;

    if(m_ssPath.isEmpty())
        return;

    modsInfoTimerTimeout();
}

void BalanceModManager::checkDownloadingQuery()
{
    if(m_downloadingQuery.isEmpty() || m_ssLounchedState || m_modDownloadingProcessed)
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

        newModInfo.isBeta  = newObject.value("isBeta").toBool();

        if (!m_settingsController->getSettings()->showBalanceModBetaVersions && newModInfo.isBeta)
            continue;

        newModInfo.id = newObject.value("id").toInt();
        newModInfo.technicalName = newObject.value("technicalName").toString();
        newModInfo.uiName = newObject.value("uiName").toString();
        newModInfo.version = newObject.value("version").toString();
        newModInfo.isLatest = newObject.value("isActual").toBool();
        newModInfo.isInstalled = entryList.contains(newModInfo.technicalName) && entryList.contains(newModInfo.technicalName + ".module");
        newModInfo.isCurrentMod = m_currentModName.toLower() == newModInfo.technicalName.toLower();
        newModInfo.isPrevious = newObject.value("isPrevious").toBool();

        if (newModInfo.isInstalled)
            newModInfo.changelog = getChangeLogFromLocalFiles(newModInfo.technicalName);

        m_modInfoList.append(newModInfo);
    }

    if (m_templateProfilePath.isEmpty())
        receiveUpdateTemplateProfilePath(m_settingsController->getSettings()->useCustomTemplateProfilePath);

    for (int i = 0; i < m_modInfoList.count(); i++)
    {
        if (m_modInfoList.at(i).isLatest && m_modInfoList.at(i).technicalName != m_lastActualMod)
        {
            newActualModDetected(m_modInfoList.at(i).technicalName, m_modInfoList.at(i).isInstalled);

            if (!m_modInfoList.at(i).isInstalled && m_settingsController->getSettings()->autoUpdateBalanceMod)
            {
                m_modInfoList[i].downloadingProcessed = true;
                requestDownloadMod(m_modInfoList.at(i).technicalName);
            }

            if (!m_modInfoList.at(i).isInstalled && (!m_settingsController->getSettings()->autoUpdateBalanceMod || m_ssLounchedState))
                sendModReadyForInstall(m_modInfoList.at(i).uiName);
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
    data.decompressPath = m_ssPath + QDir::separator();
    data.modTechnicalName = modTechnicalName;
    data.templateProfilePath = m_templateProfilePath;
    data.ssPath = m_ssPath;

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

    emit changeLogReceived(modTechnicalName, QString::fromStdString(replyByteArray.toStdString()));

    reply->deleteLater();
}
