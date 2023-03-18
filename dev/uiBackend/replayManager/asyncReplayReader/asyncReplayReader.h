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

public slots:
    void readReplaysList(QString playbackFolder, QString findText);
    void abort();

signals:
    void sendReplaysInfo(QVector<ReplayListInfo> newReplaysList);

private:
    bool m_abort = false;
    QMutex m_mutex;
};

#endif // ASYNCREPLAYREADER_H
