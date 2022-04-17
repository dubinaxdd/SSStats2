#ifndef MODSINSTALLER_H
#define MODSINSTALLER_H

#include <QObject>

class ModsInstaller : public QObject
{
    Q_OBJECT
public:
    explicit ModsInstaller(QObject *parent = nullptr);

signals:

};

#endif // MODSINSTALLER_H
