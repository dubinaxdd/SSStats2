#include "balanceModManager.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

BalanceModManager::BalanceModManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_modsInfoRequestTimer(new QTimer(this))
{
    m_modsInfoRequestTimer->setInterval(5000);
    connect(m_modsInfoRequestTimer, &QTimer::timeout, this, &BalanceModManager::downloadModsInfo, Qt::QueuedConnection);
}

void BalanceModManager::downloadMod(QString version)
{

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

void BalanceModManager::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;

    if(m_ssPath.isEmpty())
        return;

    m_modsInfoRequestTimer->start();
    downloadModsInfo();
}

void BalanceModManager::receiveVersionsInfo(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
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

        m_modInfoList.append(newModInfo);
    }

    emit sendModsInfo(m_modInfoList);
}
