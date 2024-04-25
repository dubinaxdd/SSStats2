#ifndef ADVERTISINGPROCESSOR_H
#define ADVERTISINGPROCESSOR_H

#include <QObject>
#include <baseTypes.h>
#include <QTimer>

class AdvertisingProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AdvertisingProcessor(QObject *parent = nullptr);

signals:
    void sendAdvertisingMesssage(int room, QString text);

public slots:
    void onReplaySended();
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);

private slots:
    void onSendingTimerTimeout();

private:
    QTimer* m_sendingTimer;
    QString m_currentMessageText;
    int m_currentRoom = 0;
    ServerPlayerStats m_currentPlayer;
};

#endif // ADVERTISINGPROCESSOR_H
