#ifndef MODSPAGE_H
#define MODSPAGE_H

#include <QObject>
#include <settingsController.h>
#include <modItem.h>
#include <baseTypes.h>

class ModsPage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ModItem* russianFontsMod MEMBER m_russianFontsMod CONSTANT)
    Q_PROPERTY(ModItem* cameraMod MEMBER m_cameraMod CONSTANT)
    Q_PROPERTY(ModItem* gridHotkeysMod MEMBER m_gridHotkeysMod CONSTANT)

    Q_PROPERTY(QString unlockRacesStatus MEMBER m_unlockRacesStatus NOTIFY unlockRacesStatusChanged)

public:
    explicit ModsPage(SettingsController* settingsController, QObject *parent = nullptr);

signals:
    void startInstall(InstMod mod);
    void startUninstall(InstMod mod);

    void sendUnlockRaces();
    void unlockRacesStatusChanged();

public slots:
    void receiveDownloadProgress(InstMod mod, int progress);
    void receiveInstallCompleeted(InstMod mod);
    void receiveDownloadError(InstMod mod);
    void receiveUnlockRacesStatus(bool status);

private slots:
    void onSettingsLoaded();

public:
    Q_INVOKABLE void unlockRaces();

private:
    SettingsController* m_settingsController;

    ModItem* m_russianFontsMod;
    ModItem* m_cameraMod;
    ModItem* m_gridHotkeysMod;

    QString m_unlockRacesStatus = "";
};

#endif // MODSPAGE_H
