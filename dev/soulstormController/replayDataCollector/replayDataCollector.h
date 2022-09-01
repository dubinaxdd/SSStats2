#ifndef REPLAYDATACOLLECTOR_H
#define REPLAYDATACOLLECTOR_H

#include <QObject>

class ReplayDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit ReplayDataCollector(QObject *parent = nullptr);

signals:

};

#endif // REPLAYDATACOLLECTOR_H
