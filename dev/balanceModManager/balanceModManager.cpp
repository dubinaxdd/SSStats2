#include "balanceModManager.h"
#include "QJsonArray"
#include "QJsonDocument"

BalanceModManager::BalanceModManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    downloadVersionsInfo();
}

void BalanceModManager::downloadMod(QString version)
{

}

void BalanceModManager::downloadVersionsInfo()
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/dow_stats_balance_mod/versions.txt";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveVersionsInfo(reply);
    });
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

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "versions.txt parsing error: " + replyByteArray;
        return;
    }

    m_modVersions.clear();

    QJsonArray jsonArray = jsonDoc.array();

    for (size_t i = 0; i < jsonArray.count(); i++)
        m_modVersions.append(jsonArray.at(i).toString());

    emit sendBalanceModVersoins(m_modVersions);
}
