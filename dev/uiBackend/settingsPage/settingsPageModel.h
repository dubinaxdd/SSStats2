#ifndef SETTINGSPAGEMODEL_H
#define SETTINGSPAGEMODEL_H

#include <QObject>
#include <settingsController.h>
#include <baseTypes.h>

class SettingsPageModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool overlayVisible READ overlayVisible WRITE setOverlayVisible NOTIFY overlayVisibleChanged)
    Q_PROPERTY(bool win7SupportMode READ win7SupportMode WRITE setWin7SupportMode NOTIFY win7SupportModeChanged)
    Q_PROPERTY(bool launchGameInWindow READ launchGameInWindow WRITE setLaunchGameInWindow NOTIFY launchGameInWindowChanged)
    Q_PROPERTY(bool skipIntroVideo READ skipIntroVideo WRITE setSkipIntroVideo NOTIFY skipIntroVideoChanged)
    Q_PROPERTY(bool enableEventsSoundWhenGameMinimized READ enableEventsSoundWhenGameMinimized WRITE setEnableEventsSoundWhenGameMinimized NOTIFY enableEventsSoundWhenGameMinimizedChanged)
    Q_PROPERTY(bool enableEventsSoundWhenGameMaximized READ enableEventsSoundWhenGameMaximized WRITE setEnableEventsSoundWhenGameMaximized NOTIFY enableEventsSoundWhenGameMaximizedChanged)
    Q_PROPERTY(bool enableGameLoadEventSound READ enableGameLoadEventSound WRITE setEnableGameLoadEventSound NOTIFY enableGameLoadEventSoundChanged)
    Q_PROPERTY(bool enableGameStartEventSound READ enableGameStartEventSound WRITE setEnableGameStartEventSound NOTIFY enableGameStartEventSoundChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(bool autorun READ autorun WRITE setAutorun NOTIFY autorunChanged)
    Q_PROPERTY(bool enableAdvertising READ enableAdvertising WRITE setEnableAdvertising NOTIFY enableAdvertisingChanged)
    Q_PROPERTY(int launchMode READ launchMode WRITE setLaunchMode NOTIFY launchModeChanged)
    Q_PROPERTY(Language::LanguageEnum language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit SettingsPageModel(SettingsController* settingsController, QObject *parent = nullptr);

    bool overlayVisible() const;
    void setOverlayVisible(bool newOverlayVisible);

signals:
    void overlayVisibleChanged();
    void win7SupportModeChanged();
    void skipIntroVideoChanged();
    void enableEventsSoundWhenGameMinimizedChanged(bool state);
    void enableEventsSoundWhenGameMaximizedChanged(bool state);
    void enableGameLoadEventSoundChanged(bool state);
    void enableGameStartEventSoundChanged(bool state);
    void volumeChanged(int volume);
    void launchGameInWindowChanged();
    void currentThemeChanged();
    void autorunChanged();
    void launchModeChanged();
    void enableAdvertisingChanged();
    void languageChanged(Language::LanguageEnum language);

private slots:
    void onSettingsLoaded();
    Language::LanguageEnum parseLanguage(QString language);
    QString languageToString(Language::LanguageEnum language);

public:
    bool win7SupportMode() const;
    void setWin7SupportMode(bool newWin7SupportMode);

    bool skipIntroVideo() const;
    void setSkipIntroVideo(bool newSkipIntroVideo);

    bool enableEventsSoundWhenGameMinimized() const;
    void setEnableEventsSoundWhenGameMinimized(bool newEnableEventsSoundWhenGameMinimized);

    bool enableEventsSoundWhenGameMaximized() const;
    void setEnableEventsSoundWhenGameMaximized(bool newEnableEventsSoundWhenGameMaximized);

    bool enableGameLoadEventSound() const;
    void setEnableGameLoadEventSound(bool newEnableGameLoadEventSound);

    bool enableGameStartEventSound() const;
    void setEnableGameStartEventSound(bool newEnableGameStartEventSound);

    int volume() const;
    void setVolume(int newVolume);

    bool launchGameInWindow() const;
    void setLaunchGameInWindow(bool newLaunchGameInWindow);

    int currentTheme() const;
    void setCurrentTheme(int newCurrentTheme);

    bool autorun() const;
    void setAutorun(bool newAutorun);

    int launchMode() const;
    void setLaunchMode(int newLaunchMode);

    bool enableAdvertising() const;
    void setEnableAdvertising(bool newEnableAdvertising);

    const Language::LanguageEnum &language() const;
    void setLanguage(const Language::LanguageEnum &newLanguage);

private:
    void updateAutorunState(bool isAutorun);

private:
    SettingsController* m_settingsController;

    int m_currentTheme = 0;
    bool m_overlayVisible = true;
    bool m_win7SupportMode = false;
    bool m_launchGameInWindow = false;
    bool m_skipIntroVideo = false;
    bool m_enableEventsSoundWhenGameMinimized = true;
    bool m_enableEventsSoundWhenGameMaximized = true;
    bool m_enableGameLoadEventSound = true;
    bool m_enableGameStartEventSound = true;
    bool m_autorun = true;
    bool m_enableAdvertising = false;

    int m_volume = 100;

    Language::LanguageEnum m_language = Language::LanguageEnum::System;
    LaunchMod m_launchMode = LaunchMod::LastSelectedMod;
};

#endif // SETTINGSPAGEMODEL_H
