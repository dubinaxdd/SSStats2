#ifndef SOUNDPROCESSOR_H
#define SOUNDPROCESSOR_H

#include <QObject>
#include <baseTypes.h>

class SoundProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SoundProcessor(QObject *parent = nullptr);

public slots:
    void setEnableSoundsWhenGameMinimized(bool newEnableSoundsWhenGameMinimized);
    void setEnableSoundsWhenGameMaximized(bool newEnableSoundsWhenGameMaximized);

    void playPlayerJoinSound();
    void playPlayerLeftSound();
    void playGameStartSound();

    void setSoulstormMaximized(bool isMaximized);

    void activeIsFirstConnection();

    void receiveCurrentMissionState(SsMissionState gameCurrentState);

private:
    bool m_enableSoundsWhenGameMinimized = true;
    bool m_enableSoundsWhenGameMaximized = true;
    bool m_isFirstConnection = true;
    bool m_soulstormMaximized = false;

};

#endif // SOUNDPROCESSOR_H
