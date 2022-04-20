#ifndef MODSINSTALLER_H
#define MODSINSTALLER_H

#include <QObject>

#include <zlib.h>
#include <zconf.h>
#include <QtZlib/zlib.h>
#include <logger.h>

class ModsInstaller : public QObject
{
    Q_OBJECT

public:
    explicit ModsInstaller(QString ssPath, QObject *parent = nullptr);

signals:
    void russianFontsInstalled();

public:
    void uninstalRussianFonts();
    void installRussianFonts(QString path);


private:
    QString m_ssPath = "";

};

#endif // MODSINSTALLER_H
