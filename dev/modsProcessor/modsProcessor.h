#ifndef MODSPROCESSOR_H
#define MODSPROCESSOR_H

#include <QObject>
#include <modsDownloader.h>
#include <modsInstaller.h>
#include <baseTypes.h>


class ModsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ModsProcessor(GamePath* gamePath, QObject *parent = nullptr);

signals:
    void modInstallCompleeted(InstMod);
    void installProgress(InstMod, int);
    void downloadError(InstMod);

    void sendUnlockRacesStatus(bool);

public slots:
    void modInstallRequest(InstMod mod);
    void uninstallRequest(InstMod mod);
    void unlockRaces();
    bool checkModInstalled(InstMod mod);

private:
    GamePath* m_currentGame;
    ModsDownloader* m_modsDownloader;
    ModsInstaller* m_modsInstaller;
};

#endif // MODSPROCESSOR_H
