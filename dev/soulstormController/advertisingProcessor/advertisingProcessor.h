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

public slots:
    void onReplaySended();
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);

private:
    void joinChannel(int room, QString text);
    void sendAdvertisingMessage(int room, QString text);
    void leaveChannel(int room, QString text);

private slots:
    void startAdvertising();

private:
    SettingsController* m_settingsController;
    QTimer* m_advertisingStartDelayTimer;
    QString m_currentMessageText;
    int m_currentRoom = 0;
    ServerPlayerStats m_currentPlayer;
};

#endif // ADVERTISINGPROCESSOR_H
