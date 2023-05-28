#ifndef BALANCEMODINSTALLER_H
#define BALANCEMODINSTALLER_H

#include <QObject>

struct InstallModData
{
    QByteArray modByteArray;
    QString filePath;
    QString decompressPath;
    QString modTechnicalName;
    QString hotKeysPath;
    QString ssPath;
};


class BalanceModInstaller : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModInstaller(QObject *parent = nullptr);

public slots:
    void installMod(InstallModData data);

private:
    void installHotKeys();

signals:
    void modInstalled(QString modTechnicalName);

};

#endif // BALANCEMODINSTALLER_H
