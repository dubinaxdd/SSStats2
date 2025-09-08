#include "modsProcessor.h"
#include "windows.h"
#include <QSettings>
#include <logger.h>
#include <defines.h>
#include <QDir>

ModsProcessor::ModsProcessor(GamePath *gamePath, QObject *parent)
    : QObject(parent)
    , m_currentGame(gamePath)
    , m_modsDownloader(new ModsDownloader(this))
    , m_modsInstaller(new ModsInstaller(gamePath, this))
{
    QObject::connect(m_modsDownloader, &ModsDownloader::modDownloaded, this, [&](InstMod mod, QString path){m_modsInstaller->installMod(mod, path);}, Qt::QueuedConnection);
    QObject::connect(m_modsInstaller, &ModsInstaller::modInstalled, this, [&](InstMod mod){emit modInstallCompleeted(mod);}, Qt::QueuedConnection);
    QObject::connect(m_modsDownloader, &ModsDownloader::downloadError, this, [&](InstMod mod){emit downloadError(mod);}, Qt::QueuedConnection);
    QObject::connect(m_modsDownloader, &ModsDownloader::downladProgress, this, [&](InstMod mod, int progress){emit installProgress(mod, progress);}, Qt::QueuedConnection);
}

void ModsProcessor::modInstallRequest(InstMod mod)
{
    m_modsDownloader->downloadMod(mod);
}

void ModsProcessor::uninstallRequest(InstMod mod)
{
    m_modsInstaller->uninstallMod(mod);
}

void ModsProcessor::unlockRaces()
{
    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

    BOOL f64 = FALSE;
    bool isX64System = IsWow64Process(GetCurrentProcess(), &f64);

    QString systemPathPart = "";

    if (isX64System)
    {
        qInfo(logInfo()) << "Used x64 operation system";
        systemPathPart = "WOW6432Node\\";
    }
    else
        qInfo(logInfo()) << "Used x86 operation system";

    QFile gragicsConfig(m_currentGame->gamePath + "\\GraphicsConfig.exe");

    if(!gragicsConfig.open(QFile::ReadOnly))
    {
        qWarning(logWarning()) << "Races not unlocked";
        emit sendUnlockRacesStatus(false);
        return;
    }

    QDir dir;
    dir.mkdir(m_currentGame->gamePath + "\\DowStatsUnlocker");

    gragicsConfig.copy(m_currentGame->gamePath + "\\DowStatsUnlocker\\DarkCrusade.exe");
    gragicsConfig.copy(m_currentGame->gamePath + "\\DowStatsUnlocker\\W40k.exe");
    gragicsConfig.copy(m_currentGame->gamePath + "\\DowStatsUnlocker\\W40kWA.exe");

    gragicsConfig.close();

    QSettings vanilla_CDKEY("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War", QSettings::NativeFormat);
    vanilla_CDKEY.setValue("CDKEY", VANILLA_CD_KEY);

    QSettings vanilla_CDKEY_WXP("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War", QSettings::NativeFormat);
    vanilla_CDKEY_WXP.setValue("CDKEY_WXP", WA_CD_KEY);

    QSettings vanilla_installlocation("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War", QSettings::NativeFormat);
    vanilla_CDKEY_WXP.setValue("installlocation", m_currentGame->gamePath + "\\DowStatsUnlocker\\");

    QSettings vanilla_dawnofwar_ver("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War", QSettings::NativeFormat);
    vanilla_dawnofwar_ver.setValue("dawnofwar_ver", "1.51");

    QSettings dc_CDKEY("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War - Dark Crusade", QSettings::NativeFormat);
    dc_CDKEY.setValue("CDKEY", DC_CD_KEY);

    QSettings dc_w40kcdkey("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War - Dark Crusade", QSettings::NativeFormat);
    dc_w40kcdkey.setValue("w40kcdkey", VANILLA_CD_KEY);

    QSettings dc_wxpcdkey("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War - Dark Crusade", QSettings::NativeFormat);
    dc_wxpcdkey.setValue("wxpcdkey", WA_CD_KEY);

    QSettings dc_installlocation("HKEY_LOCAL_MACHINE\\SOFTWARE\\" + systemPathPart + "THQ\\Dawn Of War - Dark Crusade", QSettings::NativeFormat);
    dc_installlocation.setValue("installlocation", m_currentGame->gamePath + "\\DowStatsUnlocker\\");

    qInfo(logInfo()) << "Races unlocked";

    emit sendUnlockRacesStatus(true);
}

bool ModsProcessor::checkModInstalled(InstMod mod)
{
    if (mod == InstMod::TransparentCameraTrapezoid)
    {
        QFile tempfile1(m_currentGame->gamePath + "\\DXP2\\Data\\Art\\ui\\minimap\\camera.tga");
        return tempfile1.exists();
    }
}
