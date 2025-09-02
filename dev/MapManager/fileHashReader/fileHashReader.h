#ifndef FILEHASHREADER_H
#define FILEHASHREADER_H

#include <QObject>
#include <baseTypes.h>

class FileHashReader : public QObject
{
    Q_OBJECT
public:
    explicit FileHashReader(QObject *parent = nullptr);

public slots:
    void getLocalMapFilesList(QString gamePath);

signals:
    void sendLocalMapFilesList(QList<MapFileHash>);

};

#endif // FILEHASHREADER_H
