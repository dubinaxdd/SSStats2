#ifndef FILEHASHREADER_H
#define FILEHASHREADER_H

#include <QObject>
#include <baseTypes.h>

class FileHashReader : public QObject
{
    Q_OBJECT
public:
    explicit FileHashReader(QString ssPath, QObject *parent = nullptr);

public slots:
    void getLocalMapFilesList();

signals:
    void sendLocalMapFilesList(QList<MapFileHash>);

private:
    QString m_ssPath = "";


};

#endif // FILEHASHREADER_H
