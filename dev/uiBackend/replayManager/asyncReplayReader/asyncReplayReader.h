#ifndef ASYNCREPLAYREADER_H
#define ASYNCREPLAYREADER_H

#include <QObject>
#include <baseTypes.h>
#include <imageProvider.h>

class AsyncReplayReader : public QObject
{
    Q_OBJECT
public:
    explicit AsyncReplayReader(QObject *parent = nullptr);

public slots:
    void readReplaysList(QString playbackFolder, QString findText);

signals:
    void sendReplaysInfo(QVector<ReplayListInfo> newReplaysList);

};

#endif // ASYNCREPLAYREADER_H
