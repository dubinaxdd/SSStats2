#include "AbstractWebServiceProcessor.h"

AbstractWebServiceProcessor::AbstractWebServiceProcessor(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    m_serviceName = "Abstract Web Service";

    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &AbstractWebServiceProcessor::onSettingsLoaded, Qt::QueuedConnection);

    connect(&m_webSocket, &QWebSocket::connected, this, &AbstractWebServiceProcessor::onConnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::disconnected, this,  &AbstractWebServiceProcessor::onDisconnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &AbstractWebServiceProcessor::readMessage, Qt::QueuedConnection);

    connect(&m_reconnectTimer, &QTimer::timeout, this, &AbstractWebServiceProcessor::reconnect, Qt::QueuedConnection);
    connect(&m_pingTimer, &QTimer::timeout, this, &AbstractWebServiceProcessor::sendPing, Qt::QueuedConnection);
    connect(&m_pingResponseTimer, &QTimer::timeout, this, [&]{m_webSocket.close();});
}

void AbstractWebServiceProcessor::reconnect()
{
    qInfo(logInfo()) << m_serviceName << "Try reconnect";
    m_webSocket.open(QUrl(m_url));
}

void AbstractWebServiceProcessor::onConnected()
{
    m_pingTimer.start();
    m_reconnectTimer.stop();
    qInfo(logInfo()) << m_serviceName << "Connected";
}

void AbstractWebServiceProcessor::onDisconnected()
{
    m_pingTimer.stop();
    m_reconnectTimer.start();
    qInfo(logInfo()) << m_serviceName << "Disconnected";
}
