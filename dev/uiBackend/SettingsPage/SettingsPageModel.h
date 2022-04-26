#ifndef SETTINGSPAGEMODEL_H
#define SETTINGSPAGEMODEL_H

#include <QObject>
#include <settingscontroller.h>
#include <baseTypes.h>

class SettingsPageModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool russianFontsInstalledStatus MEMBER m_russianFontsInstalledStatus NOTIFY russianFontsInstallStatusChanged)
    Q_PROPERTY(QString russianFontsInstallProgress MEMBER m_russianFontsInstallProgress NOTIFY russianFontsInstallProgressChanged)
    Q_PROPERTY(bool russianFontsInstallInProcess MEMBER m_russianFontsInstallInProcess NOTIFY russianFontsInstallInProcessChanged)

    Q_PROPERTY(bool cameraModInstalledStatus MEMBER m_cameraModInstalledStatus NOTIFY cameraModInstallStatusChanged)
    Q_PROPERTY(QString cameraModInstallProgress MEMBER m_cameraModInstallProgress NOTIFY cameraModInstallProgressChanged)
    Q_PROPERTY(bool cameraModInstallInProcess MEMBER m_cameraModInstallInProcess NOTIFY cameraModInstallInProcessChanged)

    Q_PROPERTY(bool gridHotkeysInstalledStatus MEMBER m_gridHotkeysInstalledStatus NOTIFY gridHotkeysInstallStatusChanged)
    Q_PROPERTY(QString gridHotkeysInstallProgress MEMBER m_gridHotkeysInstallProgress NOTIFY gridHotkeysInstallProgressChanged)
    Q_PROPERTY(bool gridHotkeysInstallInProcess MEMBER m_gridHotkeysInstallInProcess NOTIFY gridHotkeysInstallInProcessChanged)

    Q_PROPERTY(bool overlayVisible READ overlayVisible WRITE setOverlayVisible NOTIFY overlayVisibleChanged)
    Q_PROPERTY(bool win7SupportMode READ win7SupportMode WRITE setWin7SupportMode NOTIFY win7SupportModeChanged)
    Q_PROPERTY(bool skipIntroVideo READ skipIntroVideo WRITE setSkipIntroVideo NOTIFY skipIntroVideoChanged)


public:
    explicit SettingsPageModel(SettingsController* settingsController, QObject *parent = nullptr);

    bool overlayVisible() const;
    void setOverlayVisible(bool newOverlayVisible);


signals:
    void startInstall(InstMod mod);
    void startUninstall(InstMod mod);

    void russianFontsInstallStatusChanged();
    void russianFontsInstallProgressChanged();
    void russianFontsInstallInProcessChanged();

    void cameraModInstallStatusChanged();
    void cameraModInstallProgressChanged();
    void cameraModInstallInProcessChanged();

    void gridHotkeysInstallStatusChanged();
    void gridHotkeysInstallProgressChanged();
    void gridHotkeysInstallInProcessChanged();

    void overlayVisibleChanged();
    void win7SupportModeChanged();
    void skipIntroVideoChanged();

public slots:
    void receiveDownloadProgress(InstMod mod, int progress);
    void receiveInstallCompleeted(InstMod mod);
    void receiveDownloadError(InstMod mod);

private slots:
    void onSettingsLoaded();

public:
    Q_INVOKABLE void installRussianFonts();
    Q_INVOKABLE void uninstallRussianFonts();

    Q_INVOKABLE void installCameraMod();
    Q_INVOKABLE void uninstallCameraMod();

    Q_INVOKABLE void installGridHotkeys();
    Q_INVOKABLE void uninstallGridHotkeys();

    bool win7SupportMode() const;
    void setWin7SupportMode(bool newWin7SupportMode);

    bool skipIntroVideo() const;
    void setSkipIntroVideo(bool newSkipIntroVideo);

private:
    void receiveRussianFontsDownloadProgress(int progress);
    void receiveRussianFontsInstallCompleeted();
    void receiveRussianFontsDownloadError();

    void receiveCameraModDownloadProgress(int progress);
    void receiveCameraModInstallCompleeted();
    void receiveCameraModDownloadError();

    void receiveGridHotkeysDownloadProgress(int progress);
    void receiveGridHotkeysInstallCompleeted();
    void receiveGridHotkeysDownloadError();

private:
    SettingsController* m_settingsController;

    QString m_russianFontsInstallProgress = "Not installed";
    bool m_russianFontsInstalledStatus = false;
    bool m_russianFontsInstallInProcess = false;

    QString m_cameraModInstallProgress = "Not installed";
    bool m_cameraModInstalledStatus = false;
    bool m_cameraModInstallInProcess = false;

    QString m_gridHotkeysInstallProgress = "Not installed";
    bool m_gridHotkeysInstalledStatus = false;
    bool m_gridHotkeysInstallInProcess = false;

    bool m_overlayVisible = true;
    bool m_win7SupportMode = false;
    bool m_skipIntroVideo = false;

};

#endif // SETTINGSPAGEMODEL_H
