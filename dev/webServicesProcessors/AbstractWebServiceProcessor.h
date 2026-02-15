#ifndef ABSTRACTWEBSERVICEPROCESSOR_H
#define ABSTRACTWEBSERVICEPROCESSOR_H

#include <QObject>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>
#include <QWebSocket>
#include <settingsController.h>

class AbstractWebServiceProcessor : public QObject
{
public:
    AbstractWebServiceProcessor(SettingsController* settingsController, QObject *parent = nullptr);
    virtual ~AbstractWebServiceProcessor(){};

protected slots:
    virtual void onSettingsLoaded() = 0;
    virtual void readMessage(QString messgae) = 0;
    virtual void onConnected();
    void onDisconnected();
    virtual void sendPing() = 0;

    void reconnect();

protected:
    QWebSocket m_webSocket;
    QTimer m_reconnectTimer;
    QTimer m_pingTimer;
    QTimer m_pingResponseTimer;
    SettingsController* m_settingsController;
    QString m_url;
    QString m_serviceName;
};

#endif // ABSTRACTWEBSERVICEPROCESSOR_H
