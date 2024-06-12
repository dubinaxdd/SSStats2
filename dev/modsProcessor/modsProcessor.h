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
    explicit ModsProcessor(QString ssPath, QObject *parent = nullptr);

signals:
    void modInstallCompleeted(InstMod);
    void installProgress(InstMod, int);
    void downloadError(InstMod);

    void sendUnlockRacesStatus(bool);

public slots:
    void modInstallRequest(InstMod mod);
    void uninstallRequest(InstMod mod);
    void unlockRaces();

private:
    ModsDownloader* m_modsDownloader;
    ModsInstaller* m_modsInstaller;

    QString m_ssPath = "";

};

#endif // MODSPROCESSOR_H
