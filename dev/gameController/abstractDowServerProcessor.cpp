#include "abstractDowServerProcessor.h"
#include <QNetworkReply>
#include <QDebug>
#include <defines.h>

AbstractDowServerProcessor::AbstractDowServerProcessor(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

bool AbstractDowServerProcessor::checkReplyErrors(QString funcName, QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << funcName + ":" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return true;
    }

    return false;
}

QNetworkRequest AbstractDowServerProcessor::createDowServerRequest(QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url));

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        newRequest.setRawHeader("Cookie", "reliclink=" + QString(RELICLINK).toLocal8Bit() + "; AWSELB=" + QString(AWSELB).toLocal8Bit());
        newRequest.setRawHeader("Host", "dow-api.reliclink.com");
        newRequest.setRawHeader("Content-Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
    }
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        newRequest.setRawHeader("Cookie", "reliclink=" + QString(RELICLINK).toLocal8Bit() + "; AWSELB=" + QString(AWSELB).toLocal8Bit());
        newRequest.setRawHeader("Host", "dow-api.reliclink.com");
        newRequest.setRawHeader("Content-Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
    }

    QSslConfiguration sslConf;
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    newRequest.setSslConfiguration(sslConf);

    return newRequest;
}

void AbstractDowServerProcessor::setGameType(GameType::GameTypeEnum newGameType)
{
    m_gameType = newGameType;
}

void AbstractDowServerProcessor::setRequestParametres(DowServerRequestParametres parametres)
{
    m_parametres = parametres;
}

void AbstractDowServerProcessor::setSessionId(QString sessionId)
{
    m_sessionId = sessionId;
}

AbstractDowServerProcessor::~AbstractDowServerProcessor() {}
