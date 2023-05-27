#ifndef BALANCEMODINSTALLER_H
#define BALANCEMODINSTALLER_H

#include <QObject>

class BalanceModInstaller : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModInstaller(QObject *parent = nullptr);

public slots:
    void installMod(QByteArray modByteArray, QString filePath, QString decompressPath, QString modTechnicalName);

signals:
    void modInstalled(QString modTechnicalName);

};

#endif // BALANCEMODINSTALLER_H
