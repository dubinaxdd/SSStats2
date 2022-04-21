#include "ModsProcessor.h"

ModsProcessor::ModsProcessor(QString ssPath, QObject *parent)
    : QObject(parent)
    , m_modsDownloader(new ModsDownloader(this))
    , m_modsInstaller(new ModsInstaller(ssPath, this))
{
    m_ssPath = ssPath;

    QObject::connect(m_modsDownloader, &ModsDownloader::modDownloaded, this, [&](InstMod mod, QString path){m_modsInstaller->installMod(mod, path);}, Qt::QueuedConnection);
    QObject::connect(m_modsInstaller, &ModsInstaller::modInstalled, this, [&](InstMod mod){emit modInstallCompleeted(mod);}, Qt::QueuedConnection);
    QObject::connect(m_modsDownloader, &ModsDownloader::downloadError, this, [&](InstMod mod){emit downloadError(mod);}, Qt::QueuedConnection);
    QObject::connect(m_modsDownloader, &ModsDownloader::downladProgress, this, [&](InstMod mod, int progress){emit installProgress(mod, progress);}, Qt::QueuedConnection);

}

void ModsProcessor::onModInstallRequest(InstMod mod)
{
    m_modsDownloader->downloadMod(mod);
}

void ModsProcessor::onUninstallRequest(InstMod mod)
{
    m_modsInstaller->uninstallMod(mod);
}
