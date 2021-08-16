#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QObject>

class GamePanel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentApm MEMBER m_currentApm NOTIFY currentApmUpdate)
    Q_PROPERTY(QString averageApm MEMBER m_averageApm NOTIFY averageApmUpdate)

public:
    explicit GamePanel(QObject *parent = nullptr);

signals:
    void currentApmUpdate();
    void averageApmUpdate();

public slots:
    void onCurrentApmChanged(quint64 val);
    void onAverageApmChanged(quint64 val);
    void onGameStopped();

private:
    QString m_currentApm;
    QString m_averageApm;
};

#endif // GAMEPANEL_H
