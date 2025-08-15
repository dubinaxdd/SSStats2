#ifndef FILEHASHREADER_H
#define FILEHASHREADER_H

#include <QObject>
#include <baseTypes.h>

class FileHashReader : public QObject
{
    Q_OBJECT
public:
    explicit FileHashReader(GamePath* currentGame, QObject *parent = nullptr);

public slots:
    void getLocalMapFilesList();

signals:
    void sendLocalMapFilesList(QList<MapFileHash>);

private:
    GamePath* m_currentGame;


};

#endif // FILEHASHREADER_H
