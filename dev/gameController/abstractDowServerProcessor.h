#ifndef ABSTRACTDOWSERVERPROCESSOR_H
#define ABSTRACTDOWSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>

class AbstractDowServerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDowServerProcessor(QObject *parent = nullptr);
    virtual ~AbstractDowServerProcessor() = 0;

    virtual void setGameType(GameType newGameType);

public slots:
    void setDeowServerRequestParametres(DowServerRequestParametres parametres);

protected:
    bool checkReplyErrors(QString funcName, QNetworkReply *reply);
    QNetworkRequest createDowServerRequest(QString url);

protected:
    QNetworkAccessManager *m_networkManager;
    DowServerRequestParametres m_parametres;
    GameType m_gameType;

};

#endif // ABSTRACTDOWSERVERPROCESSOR_H
