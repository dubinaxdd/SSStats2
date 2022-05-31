#include "SoundProcessor.h"
#include <QSound>

SoundProcessor::SoundProcessor(QObject *parent) : QObject(parent)
{

}

void SoundProcessor::playPlayerJoinSound()
{
    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
    {
        m_isFirstConnection = false;
        return;
    }

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized) && !m_isFirstConnection)
        QSound::play(":/sounds/resources/sounds/player_joined.wav");

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
        QSound::play(":/sounds/resources/sounds/player_left.wav");

     m_isFirstConnection = false;
}

void SoundProcessor::playGameStartSound()
{
    if ((m_soulstormMaximized && !m_enableSoundsWhenGameMaximized) || (!m_soulstormMaximized && !m_enableSoundsWhenGameMinimized))
        return;

    if ((m_enableSoundsWhenGameMinimized || m_enableSoundsWhenGameMaximized))
        QSound::play(":/sounds/resources/sounds/game_started.wav");
}

void SoundProcessor::setSoulstormMaximized(bool isMaximized)
{
    m_soulstormMaximized = isMaximized;
}

void SoundProcessor::activeIsFirstConnection()
{
    m_isFirstConnection = true;
}

void SoundProcessor::receiveCurrentMissionState(SsMissionState gameCurrentState)
{
    if (gameCurrentState == SsMissionState::gameLoadStarted)
        playGameStartSound();
}
