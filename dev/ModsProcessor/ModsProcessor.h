#ifndef MODSPROCESSOR_H
#define MODSPROCESSOR_H

#include <QObject>
#include <ModsDownloader.h>
#include <ModsInstaller.h>


class ModsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ModsProcessor(QObject *parent = nullptr);

signals:


private:

    ModsDownloader* m_modsDownloader;
    ModsInstaller* m_modsInstaller;

};

#endif // MODSPROCESSOR_H
