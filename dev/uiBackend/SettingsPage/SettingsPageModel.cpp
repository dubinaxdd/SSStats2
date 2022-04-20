#include "SettingsPageModel.h"
#include <QDebug>

SettingsPageModel::SettingsPageModel(QObject *parent) : QObject(parent)
{

}

void SettingsPageModel::installRussianFonts()
{
    m_russianFontsInstallProgress = "Progress: 0%";
    emit russianFontsInstallProgressChanged();
    emit startRussianFontsInstall();
}

void SettingsPageModel::receiveRussianFontsDownloadProgress(int progress)
{
    m_russianFontsInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit russianFontsInstallProgressChanged();
}

void SettingsPageModel::receiveRussianFontsInstallCompleeted()
{
    m_russianFontsInstalledStatus = true;
    m_russianFontsInstallProgress = "Installed";

    emit russianFontsInstallProgressChanged();
    emit russianFontsInstallStatusChanged();
}
