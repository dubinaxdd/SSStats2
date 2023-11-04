#include "informationPage.h"
#include <QDebug>

InformationPage::InformationPage(QObject *parent) : QObject(parent)
{

}

int InformationPage::getRank7Min()
{
    return m_rankDiversion.rank7Min;
}

int InformationPage::getRank7Max()
{
    return m_rankDiversion.rank7Max;
}

int InformationPage::getRank6Min()
{
    return m_rankDiversion.rank6Min;
}

int InformationPage::getRank6Max()
{
    return m_rankDiversion.rank6Max;
}

int InformationPage::getRank5Min()
{
    return m_rankDiversion.rank5Min;
}

int InformationPage::getRank5Max()
{
    return m_rankDiversion.rank5Max;
}

int InformationPage::getRank4Min()
{
    return m_rankDiversion.rank4Min;
}

int InformationPage::getRank4Max()
{
    return m_rankDiversion.rank4Max;
}

int InformationPage::getRank3Min()
{
    return m_rankDiversion.rank3Min;
}

int InformationPage::getRank3Max()
{
    return m_rankDiversion.rank3Max;
}

int InformationPage::getRank2Min()
{
    return m_rankDiversion.rank2Min;
}

int InformationPage::getRank2Max()
{
    return m_rankDiversion.rank2Max;
}

int InformationPage::getRank1Min()
{
    return m_rankDiversion.rank1Min;
}

int InformationPage::getRank1Max()
{
    return m_rankDiversion.rank1Max;
}

void InformationPage::receiveRankDiversion(RankDiversion rankDiversion)
{
    m_rankDiversion = rankDiversion;

    emit rankDiversionChanged();
}
