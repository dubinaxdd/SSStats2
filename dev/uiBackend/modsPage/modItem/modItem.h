#ifndef MODITEM_H
#define MODITEM_H

#include <QObject>

class ModItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString installProgress READ installProgress NOTIFY installProgressChanged)
    Q_PROPERTY(bool installedStatus READ installedStatus NOTIFY installedStatusChanged)
    Q_PROPERTY(bool installInProcess READ installInProcess NOTIFY installInProcessChanged)

public:
    explicit ModItem(QObject *parent = nullptr);

    bool installedStatus() const;
    bool installInProcess() const;
    const QString &installProgress() const;

    void setDownloadProgress(int progress);
    void setInstallCompleeted();
    void setDownloadError();

    void setInstalledStatus(bool newInstalledStatus);

    Q_INVOKABLE void installMod();
    Q_INVOKABLE void uninstallMod();

signals:
    void installedStatusChanged();
    void installInProcessChanged();
    void installProgressChanged();

    void startInstall();
    void startUninstall();

private:

    void setInstallInProcess(bool newInstallInProcess);
    void setInstallProgress(const QString &newInstallProgress);

private:
    QString m_installProgress = tr("Not installed");
    bool m_installedStatus = false;
    bool m_installInProcess = false;
};

#endif // MODITEM_H
