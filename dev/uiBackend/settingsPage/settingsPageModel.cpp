#include "settingsPageModel.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>

SettingsPageModel::SettingsPageModel(SoundProcessor *soundProcessor, SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_soundProcessorPtr(soundProcessor)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SettingsPageModel::onSettingsLoaded, Qt::QueuedConnection);
}

void SettingsPageModel::onSettingsLoaded()
{
    qInfo(logInfo()) << "SettingsPageModel::onSettingsLoaded()" << "load started";

    m_enableEventsSoundWhenGameMinimized = m_settingsController->getSettings()->enableEventsSoundWhenGameMinimized;
    m_soundProcessorPtr->setEnableSoundsWhenGameMinimized(m_enableEventsSoundWhenGameMinimized);
    emit enableEventsSoundWhenGameMinimizedChanged();

    m_enableEventsSoundWhenGameMaximized = m_settingsController->getSettings()->enableEventsSoundWhenGameMaximized;
    m_soundProcessorPtr->setEnableSoundsWhenGameMaximized(m_enableEventsSoundWhenGameMaximized);
    emit enableEventsSoundWhenGameMaximizedChanged();

    m_enableGameLoadEventSound = m_settingsController->getSettings()->enableGameLoadEventSound;
    m_soundProcessorPtr->setEnableGameLoadEventSound(m_enableGameLoadEventSound);
    emit enableGameLoadEventSoundChanged();

    m_enableGameStartEventSound = m_settingsController->getSettings()->enableGameStartEventSound;
    m_soundProcessorPtr->setEnableGameStartEventSound(m_enableGameStartEventSound);
    emit enableGameStartEventSoundChanged();

    m_volume = m_settingsController->getSettings()->volume;
    m_soundProcessorPtr->setVolume(m_volume);
    emit volumeChanged(m_volume);

    m_overlayVisible =  m_settingsController->getSettings()->overlayVisible;
    emit overlayVisibleChanged();

    m_win7SupportMode =  m_settingsController->getSettings()->win7SupportMode;
    emit win7SupportModeChanged();

    m_skipIntroVideo =  m_settingsController->getSettings()->skipIntroVideo;
    emit skipIntroVideoChanged();

    m_launchGameInWindow = m_settingsController->getSettings()->launchGameInWindow;
    emit launchGameInWindowChanged();

    m_currentTheme = m_settingsController->getSettings()->currentTheme;
    emit currentThemeChanged();

    m_autorun = m_settingsController->getSettings()->autorun;
    emit autorunChanged();

    m_launchMode =  m_settingsController->getSettings()->launchMode;
    emit launchModeChanged();

    m_enableAdvertising = m_settingsController->getSettings()->enableAdvertising;
    emit enableAdvertisingChanged();

    m_language = parseLanguage(m_settingsController->getSettings()->language);
    emit languageChanged(m_language);

    updateAutorunState(m_autorun);

    qInfo(logInfo()) << "SettingsPageModel::onSettingsLoaded()" << "load finished";
}

Language::LanguageEnum SettingsPageModel::parseLanguage(QString language)
{
    if(language == "ru")
        return Language::LanguageEnum::Ru;

    if(language == "eng")
        return Language::LanguageEnum::Eng;

    return Language::LanguageEnum::System;
}

QString SettingsPageModel::languageToString(Language::LanguageEnum language)
{
    switch (language)
    {
        case Language::LanguageEnum::Eng: return "eng";
        case Language::LanguageEnum::Ru: return "ru";
        default: return "system";
    }
}

bool SettingsPageModel::launchGameInWindow() const
{
    return m_launchGameInWindow;
}

void SettingsPageModel::setLaunchGameInWindow(bool newLaunchGameInWindow)
{
    if (m_launchGameInWindow == newLaunchGameInWindow)
        return;
    m_launchGameInWindow = newLaunchGameInWindow;
    emit launchGameInWindowChanged();

    m_settingsController->getSettings()->launchGameInWindow = m_launchGameInWindow;
    m_settingsController->saveSettings();
}

int SettingsPageModel::currentTheme() const
{
    return m_currentTheme;
}

void SettingsPageModel::setCurrentTheme(int newCurrentTheme)
{
    if (m_currentTheme == newCurrentTheme)
        return;
    m_currentTheme = newCurrentTheme;
    emit currentThemeChanged();

    m_settingsController->getSettings()->currentTheme = m_currentTheme;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::autorun() const
{
    return m_autorun;
}

void SettingsPageModel::setAutorun(bool newAutorun)
{
    if (m_autorun == newAutorun)
        return;

    m_autorun = newAutorun;
    updateAutorunState(m_autorun);

    m_settingsController->getSettings()->autorun = m_autorun;
    m_settingsController->saveSettings();
}

void SettingsPageModel::updateAutorunState(bool isAutorun)
{
    QSettings bootUpSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

     if (isAutorun)
        bootUpSettings.setValue("DowStatsAutorun",  QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+ "\\DowStatsAutorun.exe"));
     else
        bootUpSettings.remove("DowStatsAutorun");

     bootUpSettings.sync();
}

const Language::LanguageEnum &SettingsPageModel::language() const
{
    return m_language;
}

void SettingsPageModel::setLanguage(const Language::LanguageEnum &newLanguage)
{
    if (m_language == newLanguage)
        return;
    m_language = newLanguage;
    emit languageChanged(m_language);

    m_settingsController->getSettings()->language = languageToString(m_language);
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableAdvertising() const
{
    return m_enableAdvertising;
}

void SettingsPageModel::setEnableAdvertising(bool newEnableAdvertising)
{
    if (m_enableAdvertising == newEnableAdvertising)
        return;
    m_enableAdvertising = newEnableAdvertising;
    emit enableAdvertisingChanged();

    m_settingsController->getSettings()->enableAdvertising = m_enableAdvertising;
    m_settingsController->saveSettings();
}

int SettingsPageModel::volume() const
{
    return m_volume;
}

void SettingsPageModel::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;
    m_soundProcessorPtr->setVolume(m_volume);
    emit volumeChanged(m_volume);

    m_settingsController->getSettings()->volume = m_volume;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableGameStartEventSound() const
{
    return m_enableGameStartEventSound;
}

void SettingsPageModel::setEnableGameStartEventSound(bool newEnableGameStartEventSound)
{
    if (m_enableGameStartEventSound == newEnableGameStartEventSound)
        return;
    m_enableGameStartEventSound = newEnableGameStartEventSound;
    m_soundProcessorPtr->setEnableGameStartEventSound(m_enableGameStartEventSound);
    emit enableGameStartEventSoundChanged();

    m_settingsController->getSettings()->enableGameStartEventSound = m_enableGameStartEventSound;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableGameLoadEventSound() const
{
    return m_enableGameLoadEventSound;
}

void SettingsPageModel::setEnableGameLoadEventSound(bool newEnableGameLoadEventSound)
{
    if (m_enableGameLoadEventSound == newEnableGameLoadEventSound)
        return;
    m_enableGameLoadEventSound = newEnableGameLoadEventSound;
    m_soundProcessorPtr->setEnableGameLoadEventSound(m_enableGameLoadEventSound);
    emit enableGameLoadEventSoundChanged();

    m_settingsController->getSettings()->enableGameLoadEventSound = m_enableGameLoadEventSound;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableEventsSoundWhenGameMaximized() const
{
    return m_enableEventsSoundWhenGameMaximized;
}

void SettingsPageModel::setEnableEventsSoundWhenGameMaximized(bool newEnableEventsSoundWhenGameMaximized)
{
    if (m_enableEventsSoundWhenGameMaximized == newEnableEventsSoundWhenGameMaximized)
        return;
    m_enableEventsSoundWhenGameMaximized = newEnableEventsSoundWhenGameMaximized;
    m_soundProcessorPtr->setEnableSoundsWhenGameMaximized(m_enableEventsSoundWhenGameMaximized);
    emit enableEventsSoundWhenGameMaximizedChanged();

    m_settingsController->getSettings()->enableEventsSoundWhenGameMaximized = m_enableEventsSoundWhenGameMaximized;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableEventsSoundWhenGameMinimized() const
{
    return m_enableEventsSoundWhenGameMinimized;
}

void SettingsPageModel::setEnableEventsSoundWhenGameMinimized(bool newEnableEventsSoundWhenGameMinimized)
{
    if (m_enableEventsSoundWhenGameMinimized == newEnableEventsSoundWhenGameMinimized)
        return;

    m_enableEventsSoundWhenGameMinimized = newEnableEventsSoundWhenGameMinimized;
    m_soundProcessorPtr->setEnableSoundsWhenGameMinimized(m_enableEventsSoundWhenGameMinimized);
    emit enableEventsSoundWhenGameMinimizedChanged();

    m_settingsController->getSettings()->enableEventsSoundWhenGameMinimized = m_enableEventsSoundWhenGameMinimized;
    m_settingsController->saveSettings();
}


bool SettingsPageModel::skipIntroVideo() const
{
    return m_skipIntroVideo;
}

void SettingsPageModel::setSkipIntroVideo(bool newSkipIntroVideo)
{
    if (m_skipIntroVideo ==  newSkipIntroVideo)
        return;

    m_skipIntroVideo = newSkipIntroVideo;
    emit skipIntroVideoChanged();

    m_settingsController->getSettings()->skipIntroVideo = m_skipIntroVideo;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::win7SupportMode() const
{
    return m_win7SupportMode;
}

void SettingsPageModel::setWin7SupportMode(bool newWin7SupportMode)
{
    if (m_win7SupportMode == newWin7SupportMode)
        return;
    m_win7SupportMode = newWin7SupportMode;
    emit win7SupportModeChanged();

    m_settingsController->getSettings()->win7SupportMode = m_win7SupportMode;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::overlayVisible() const
{
    return m_overlayVisible;
}

void SettingsPageModel::setOverlayVisible(bool newOverlayVisible)
{
    if (m_overlayVisible == newOverlayVisible)
        return;
    m_overlayVisible = newOverlayVisible;
    emit overlayVisibleChanged();

    m_settingsController->getSettings()->overlayVisible = m_overlayVisible;
    m_settingsController->saveSettings();
}

int SettingsPageModel::launchMode() const
{
    return m_launchMode;
}

void SettingsPageModel::setLaunchMode(int newLaunchMode)
{
    if (m_launchMode == newLaunchMode)
        return;

    m_launchMode = static_cast<LaunchMod>(newLaunchMode);
    emit launchModeChanged();

    m_settingsController->getSettings()->launchMode = m_launchMode;
    m_settingsController->saveSettings();
}


