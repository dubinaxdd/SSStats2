#include "modItem.h"

ModItem::ModItem(QObject *parent) : QObject(parent)
{

}

bool ModItem::installedStatus() const
{
    return m_installedStatus;
}

void ModItem::setInstalledStatus(bool newInstalledStatus)
{
    if (m_installedStatus == newInstalledStatus)
        return;

    m_installedStatus = newInstalledStatus;
    emit installedStatusChanged();

    if(m_installedStatus)
        setInstallProgress(tr("Installed"));
    else
        setInstallProgress(tr("Not installed"));
}

bool ModItem::installInProcess() const
{
    return m_installInProcess;
}

void ModItem::setInstallInProcess(bool newInstallInProcess)
{
    if (m_installInProcess == newInstallInProcess)
        return;
    m_installInProcess = newInstallInProcess;
    emit installInProcessChanged();
}

const QString &ModItem::installProgress() const
{
    return m_installProgress;
}

void ModItem::setDownloadProgress(int progress)
{
    setInstallProgress(tr("Progress: ") + QString::number(progress) + "%");
}

void ModItem::setInstallCompleeted()
{
    setInstalledStatus(true);
    setInstallInProcess(false);
    setInstallProgress(tr("Installed"));
}

void ModItem::setDownloadError()
{
    setInstallInProcess(false);
    setInstallProgress(tr("Download error"));
}

void ModItem::setInstallProgress(const QString &newInstallProgress)
{
    if (m_installProgress == newInstallProgress)
        return;
    m_installProgress = newInstallProgress;
    emit installProgressChanged();
}

void ModItem::installMod()
{
    setInstallInProcess(true);
    setInstallProgress(tr("Progress: ") + "0%");
    emit startInstall();
}

void ModItem::uninstallMod()
{
    setInstalledStatus(false);
    setInstallProgress(tr("Not installed"));
    emit startUninstall();
}
