#ifndef BALANCEMODINSTALLER_H
#define BALANCEMODINSTALLER_H

#include <QObject>

struct InstallModData
{
    QByteArray modByteArray;
    QString filePath;
    QString decompressPath;
    QString modTechnicalName;
    QString templateProfilePath;
    QString ssPath;
};


class BalanceModInstaller : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModInstaller(QObject *parent = nullptr);

public slots:
    void installMod(InstallModData data, bool overwriteProfiles);
    void uninstallMod(QString ssPath, QString modTechnicalName);
    void updateHotKeysOnMod(QString modTechnicalName, QString ssPath);

private:
    void installHotKeys(QString hotkeysDir, QString templateHotKeyPath, QString missingHotKeysPath);
    bool updateHotkeys(QString hotkeysPath, QString missingHotKeysPath);

signals:
    void modInstalled(QString modTechnicalName);
    void modInstallError(QString modTechnicalName);
    void modUninstalled(QString modTechnicalName);
    void hotKeysUpdated(QString modTechnicalName, bool result);
};

#endif // BALANCEMODINSTALLER_H
