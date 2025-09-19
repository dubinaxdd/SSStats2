#ifndef MODSINSTALLER_H
#define MODSINSTALLER_H

#include <QObject>

#include <zlib.h>
#include <zconf.h>
#include <QtZlib/zlib.h>
#include <logger.h>
#include <baseTypes.h>

class ModsInstaller : public QObject
{
    Q_OBJECT

public:
    explicit ModsInstaller(GamePath* gamePath, QObject *parent = nullptr);

signals:
    void modInstalled(InstMod mod);

public:
    void uninstallMod(InstMod mod);
    void installMod(InstMod mod, QString path);

private:
    void installRussianFonts(QString path);
    void uninstallRussianFonts();

    void installCameraMod(QString path);
    void uninstallCameraMod();

    void installGridHotkeys(QString path);
    void uninstallGridHotkeys();

    void installTransparentCameraTrapezoid(QString path);
    void uninstallTransparentCameraTrapezoid();


private:
    GamePath* m_currentGame;

};

#endif // MODSINSTALLER_H
