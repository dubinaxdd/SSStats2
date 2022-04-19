#include "ModsProcessor.h"

ModsProcessor::ModsProcessor(QObject *parent)
    : QObject(parent)
    , m_modsDownloader(new ModsDownloader(this))
    , m_modsInstaller(new ModsInstaller(this))
{
    m_modsDownloader->downloadRussinFonts();
}
