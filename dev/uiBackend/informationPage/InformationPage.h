#ifndef INFORMATIONPAGE_H
#define INFORMATIONPAGE_H

#include <QObject>
#include <baseTypes.h>

class InformationPage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int rank7Min READ getRank7Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank7Max READ getRank7Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank6Min READ getRank6Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank6Max READ getRank6Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank5Min READ getRank5Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank5Max READ getRank5Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank4Min READ getRank4Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank4Max READ getRank4Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank3Min READ getRank3Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank3Max READ getRank3Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank2Min READ getRank2Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank2Max READ getRank2Max NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank1Min READ getRank1Min NOTIFY rankDiversionChanged)
    Q_PROPERTY(int rank1Max READ getRank1Max NOTIFY rankDiversionChanged)

public:
    explicit InformationPage(QObject *parent = nullptr);\

    int getRank7Min();
    int getRank7Max();
    int getRank6Min();
    int getRank6Max();
    int getRank5Min();
    int getRank5Max();
    int getRank4Min();
    int getRank4Max();
    int getRank3Min();
    int getRank3Max();
    int getRank2Min();
    int getRank2Max();
    int getRank1Min();
    int getRank1Max();

public slots:
    void receiveRankDiversion(RankDiversion rankDiversion);

signals:
    void rankDiversionChanged();

private:
    RankDiversion m_rankDiversion;

};

#endif // INFORMATIONPAGE_H
