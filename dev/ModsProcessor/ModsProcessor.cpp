#include "ModsProcessor.h"

ModsProcessor::ModsProcessor(QString ssPath, QObject *parent)
    : QObject(parent)
    , m_modsDownloader(new ModsDownloader(this))
    , m_modsInstaller(new ModsInstaller(ssPath, this))
{
    m_ssPath = ssPath;

    QObject::connect(m_modsDownloader, &ModsDownloader::russianFontsDownloaded, this, [&](QString path){m_modsInstaller->installRussianFonts(path);}, Qt::QueuedConnection);
    QObject::connect(m_modsInstaller, &ModsInstaller::russianFontsInstalled, this, [&]{emit russianFontsInstallCompleeted();}, Qt::QueuedConnection);
    QObject::connect(m_modsDownloader, &ModsDownloader::russianFontsDownladProgress, this, [&](int progress){emit russianFontsInstallProgress(progress);}, Qt::QueuedConnection);

}

void ModsProcessor::onRussianFontsInstallRequest()
{
    m_modsDownloader->downloadRussinFonts();
}

void ModsProcessor::onRussianFontsUninstallRequest()
{
    m_modsInstaller->uninstalRussianFonts();
}
