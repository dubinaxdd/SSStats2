#ifndef MODSPROCESSOR_H
#define MODSPROCESSOR_H

#include <QObject>
#include <ModsDownloader.h>
#include <ModsInstaller.h>



class ModsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ModsProcessor(QString ssPath, QObject *parent = nullptr);

signals:
    void russianFontsInstallCompleeted();
    void russianFontsInstallProgress(int);
    void russianFontsDownloadError();

public slots:
    void onRussianFontsInstallRequest();
    void onRussianFontsUninstallRequest();

private:
    ModsDownloader* m_modsDownloader;
    ModsInstaller* m_modsInstaller;

    QString m_ssPath = "";

};

#endif // MODSPROCESSOR_H
