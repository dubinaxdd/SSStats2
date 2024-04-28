#ifndef ADVERTISINGPROCESSOR_H
#define ADVERTISINGPROCESSOR_H

#include <QObject>
#include <baseTypes.h>
#include <QTimer>
#include <settingsController/settingsController.h>
#include <abstractDowServerProcessor.h>

class AdvertisingProcessor : public AbstractDowServerProcessor
{
    Q_OBJECT
public:
    explicit AdvertisingProcessor(SettingsController* settingsController, QObject *parent = nullptr);

    enum RequestTypeEnum{
        JoinChannel = 0,
        SendAdvertisingMessage,
        LeaveChannel
    };

public slots:
    void onReplaySended();
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);

private:
    void joinChannel();
    void sendAdvertisingMessage();
    void leaveChannel();

private slots:
    void startAdvertising();
    void sendNextRequest();

private:
    SettingsController* m_settingsController;
    QTimer* m_delayTimer;
    QTimer* m_requestTimer;
    QString m_currentMessageText;
    QString m_currentText;
    int m_currentRoom = 0;
    RequestTypeEnum m_currentRequestType = JoinChannel;
    ServerPlayerStats m_currentPlayer;
    int m_playerRealRoom = 0;
};

#endif // ADVERTISINGPROCESSOR_H
