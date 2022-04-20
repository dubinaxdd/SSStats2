#ifndef SETTINGSPAGEMODEL_H
#define SETTINGSPAGEMODEL_H

#include <QObject>
#include <settingscontroller.h>

class SettingsPageModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool russianFontsInstalledStatus MEMBER m_russianFontsInstalledStatus NOTIFY russianFontsInstallStatusChanged)
    Q_PROPERTY(QString russianFontsInstallProgress MEMBER m_russianFontsInstallProgress NOTIFY russianFontsInstallProgressChanged)

public:
    explicit SettingsPageModel(SettingsController* settingsController, QObject *parent = nullptr);

signals:
    void startRussianFontsInstall();
    void startRussianFontsUninstall();

    void russianFontsInstallStatusChanged();
    void russianFontsInstallProgressChanged();

public slots:
    void receiveRussianFontsDownloadProgress(int progress);
    void receiveRussianFontsInstallCompleeted();

private slots:
    void onSettingsLoaded();

public:
    Q_INVOKABLE void installRussianFonts();
    Q_INVOKABLE void uninstallRussianFonts();

private:
    SettingsController* m_settingsController;

    QString m_russianFontsInstallProgress = "Not installed";
    bool m_russianFontsInstalledStatus = false;

};

#endif // SETTINGSPAGEMODEL_H
