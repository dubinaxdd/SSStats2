#ifndef GAMEMEMORYREADER_H
#define GAMEMEMORYREADER_H

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QTimer>
#include <windows.h>
#include <baseTypes.h>
#include <QMutex>

class GameMemoryReader : public QObject
{
    Q_OBJECT
public:
    explicit GameMemoryReader(QObject *parent = nullptr);
    void setGameType(GameType::GameTypeEnum newGameType);

signals:
    void sendSessionId(QString sessionId);
    void sendSessionIdError();
    void sendPlayersIdList(QStringList playersInfo);


public slots:
    void findSessionId();
    void findIgnoredPlayersId(QStringList playerNames);
    void abort();

private:
    QString findSteamSoulstormSessionId();
    QString findDefinitiveEditionSessionId();
    QString findChecksummParameter(QByteArray *buffer, QByteArray head);

    QStringList findIgnoredPlayersIdInMemory(const QStringList& playerIdList);
    HANDLE getProcessHandle(QString gameName);

private:
    unsigned char sessionHeader[10] = { 0x73, 0x65, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x49, 0x44, 0x3D};

    HWND m_gameHwnd = NULL;
    QMutex m_mutex;
    std::atomic<bool> m_abort;
    GameType::GameTypeEnum m_gameType;
};
#endif // GAMEMEMORYREADER_H
