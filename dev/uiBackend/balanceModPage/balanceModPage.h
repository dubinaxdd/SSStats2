#ifndef BALANCEMODPAGE_H
#define BALANCEMODPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class BalanceModPage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString selectedModName READ selectedModName NOTIFY selectedModInfoChanged)
    Q_PROPERTY(QString selectedModVersion READ selectedModVersion NOTIFY selectedModInfoChanged)
    Q_PROPERTY(QString selectedChangeLog READ selectedChangeLog NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsCurrent READ selectedModIsCurrent NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsActual READ selectedModIsActual NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsInstalled READ selectedModIsInstalled NOTIFY selectedModInfoChanged)

    Q_PROPERTY(QString downloadingProgress MEMBER m_downloadingProgress NOTIFY downloadingProgressChanged)

    Q_PROPERTY(QString currentModInGame READ currentModInGame WRITE setCurrentModInGame NOTIFY currentModInGameChanged)

public:
    explicit BalanceModPage(QObject *parent = nullptr);

    enum DataRoles {
        Selected = Qt::UserRole + 1,
        IsInstalled = Qt::UserRole + 2,
        Version = Qt::UserRole + 3,
        IsCurrentMod = Qt::UserRole + 4,
        UiName
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    Q_INVOKABLE void slectItem(int itemIndex);
    Q_INVOKABLE void downloadCurrentMod();
    Q_INVOKABLE void uninstallCurrentMod();

    const QString selectedModName() const;
    const QString selectedModVersion() const;
    const QString selectedChangeLog() const;
    const bool selectedModIsCurrent() const;
    const bool selectedModIsActual() const;
    const bool selectedModIsInstalled() const;

    const QString &currentModInGame() const;
    void setCurrentModInGame(const QString &newCurrentModInGame);

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
   void receiveVersions(QList<ModInfo> modsInfo);
   void receiveCurrentModInGame(QString modName);
   void receiveChangeLog(QString modTechnicalName, QString changeLog);
   void receiveModDownloadProgress(int progress, QString modTechnicalName);
   void receiveModDownloaded(QString modTechnicalName);

signals:
   void selectedModInfoChanged();
   void currentModInGameChanged();
   void requestChangeLog(QString modTechnicalName);
   void requestDownloadMod(QString modTechnicalName);
   void requestUninstallMod(QString modTechnicalName);
   void downloadingProgressChanged();

private:
   QList<ModInfo> m_modsInfo;
   int m_selectedItemIndex = 0;
   QString m_currentModInGame = "";

   QString m_downloadingProgress = "";
};

#endif // BALANCEMODPAGE_H
