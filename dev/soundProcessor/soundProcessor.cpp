#include "soundProcessor.h"
#include <QSound>
#include <QDebug>

SoundProcessor::SoundProcessor(QObject *parent) : QObject(parent)
{
    m_player = new QMediaPlayer(this);


}

void SoundProcessor::playPlayerJoinSound()
{
    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
    {
        m_isFirstConnection = false;
        return;
    }

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized) && !m_isFirstConnection)
        playAudio("qrc:/sounds/resources/sounds/player_joined.wav");

    m_isFirstConnection = false;


}

void SoundProcessor::playPlayerLeftSound()
{
    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
    {
        m_isFirstConnection = false;
        return;
    }

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized) && !m_isFirstConnection)
        playAudio("qrc:/sounds/resources/sounds/player_left.wav");

     m_isFirstConnection = false;
}

void SoundProcessor::playGameStartSound()
{
    if(!m_enableGameStartEventSound)
        return;

    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
        return;

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized))
        playAudio("qrc:/sounds/resources/sounds/game_started.wav");
}

void SoundProcessor::playGameLoadSound()
{
    if(!m_enableGameLoadEventSound)
        return;

    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
        return;

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized))
        playAudio("qrc:/sounds/resources/sounds/game_loading.wav");
}

void SoundProcessor::setGameMaximized(bool isMaximized)
{
    m_soulstormMaximized = isMaximized;
}

void SoundProcessor::activeIsFirstConnection()
{
    m_isFirstConnection = true;
}

void SoundProcessor::receiveCurrentMissionState(GameMissionState gameCurrentState)
{
    if (gameCurrentState == GameMissionState::gameLoadStarted)
        playGameLoadSound();

    if (gameCurrentState == GameMissionState::gameStarted)
        playGameStartSound();
}

void SoundProcessor::setVolume(int volume)
{
    m_player->setVolume(volume);
}

void SoundProcessor::playAudio(QString url)
{
    m_player->stop();
    m_player->setMedia(QUrl(url));
    m_player->play();
}

void SoundProcessor::setEnableGameStartEventSound(bool newEnableGameStartEventSound)
{
    m_enableGameStartEventSound = newEnableGameStartEventSound;
}

void SoundProcessor::setEnableGameLoadEventSound(bool newEnableGameLoadEventSound)
{
    m_enableGameLoadEventSound = newEnableGameLoadEventSound;
}

void SoundProcessor::setEnableSoundsWhenGameMaximized(bool newEnableSoundsWhenGameMaximized)
{
    m_enableSoundsWhenGameMaximized = newEnableSoundsWhenGameMaximized;
}

void SoundProcessor::setEnableSoundsWhenGameMinimized(bool newEnableSoundsWhenGameMinimized)
{
    m_enableSoundsWhenGameMinimized = newEnableSoundsWhenGameMinimized;
}
