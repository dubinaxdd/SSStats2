#ifndef ASYNCREPLAYREADER_H
#define ASYNCREPLAYREADER_H

#include <QObject>
#include <baseTypes.h>
#include <imageProvider.h>
#include <QMutex>

class AsyncReplayReader : public QObject
{
    Q_OBJECT
public:
    explicit AsyncReplayReader(QObject *parent = nullptr);
    void setModInfo(QList<ModInfo> *newModInfo);

public slots:
    void readReplaysList(QString playbackFolder, QString findText);
    void abort();

signals:
    void sendReplaysInfo(QVector<ReplayListInfo> newReplaysList);

private:
    bool m_abort = false;
    bool m_modInfoReceived = false;
    QMutex m_mutex;

    QList <ModInfo> *m_modInfo = nullptr;
};

#endif // ASYNCREPLAYREADER_H
