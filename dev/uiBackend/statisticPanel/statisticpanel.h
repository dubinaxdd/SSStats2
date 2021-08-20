#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include "../../baseTypes/baseTypes.h"
#include <QObject>
#include <QImage>
#include "../imageProvider/imageprovider.h"

class StatisticPanel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentPlayerApm MEMBER m_currentPlayerApm NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerGamesCount MEMBER m_currentPlayerGamesCount NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerMmr MEMBER m_currentPlayerMmr NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerMmr1v1 MEMBER m_currentPlayerMmr1v1 NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerName MEMBER m_currentPlayerName NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerRace MEMBER m_currentPlayerRace NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerWinRate MEMBER m_currentPlayerWinRate NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerWinsCount MEMBER m_currentPlayerWinsCount NOTIFY currentPlayerStatsChanged)

public:
    explicit StatisticPanel(ImageProvider *imageProvider, QObject *parent = nullptr);

signals:
    void currentPlayerStatsChanged();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);

private:

    ImageProvider *m_imageProvider;

    QString m_currentPlayerApm;
    QString m_currentPlayerGamesCount;
    QString m_currentPlayerMmr;
    QString m_currentPlayerMmr1v1;
    QString m_currentPlayerName;
    QString m_currentPlayerRace;
    QString m_currentPlayerWinRate;
    QString m_currentPlayerWinsCount;
};

#endif // STATISTICPANEL_H
