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

public:
    explicit SettingsPageModel(SettingsController* settingsController, QObject *parent = nullptr);

signals:
    void startInstall(InstMod mod);
    void startUninstall(InstMod mod);

    void russianFontsInstallStatusChanged();
    void russianFontsInstallProgressChanged();
    void russianFontsInstallInProcessChanged();

    void cameraModInstallStatusChanged();
    void cameraModInstallProgressChanged();
    void cameraModInstallInProcessChanged();

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

private:
    void receiveRussianFontsDownloadProgress(int progress);
    void receiveRussianFontsInstallCompleeted();
    void receiveRussianFontsDownloadError();

    void receiveCameraModDownloadProgress(int progress);
    void receiveCameraModInstallCompleeted();
    void receiveCameraModDownloadError();

private:
    SettingsController* m_settingsController;

    QString m_russianFontsInstallProgress = "Not installed";
    bool m_russianFontsInstalledStatus = false;
    bool m_russianFontsInstallInProcess = false;

    QString m_cameraModInstallProgress = "Not installed";
    bool m_cameraModInstalledStatus = false;
    bool m_cameraModInstallInProcess = false;


};

#endif // SETTINGSPAGEMODEL_H
