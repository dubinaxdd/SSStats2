#ifndef BASETYPES_H
#define BASETYPES_H

#include <QString>
#include <QImage>
#include <QDateTime>
#include <QUrl>
#include <QUuid>

class Language {
    Q_GADGET
public:
    enum LanguageEnum: int{
        System = 0,
        Eng = 1,
        Ru = 2
    };
    Q_ENUM(LanguageEnum)
private:
    Language(){}
};

class BanType {
    Q_GADGET
public:
    enum BanTypeEnum: int{
        NotBanned = 0,
        Cheater = 1,
        Banned = 2,
        FormerCheater = 3,
        SoftwareUseBan = 4
    };
    Q_ENUM(BanTypeEnum)
private:
    BanType(){}
};


class NotificationType {
    Q_GADGET
public:
    enum NotificationTypeEnum: int{
        Info = 0,
        Warning = 1,
        Downloading = 2,
        Delete = 3,
        Ready = 4,
        Update = 5
    };
    Q_ENUM(NotificationTypeEnum)
private:
    NotificationType(){}
};

/*
enum NotificationType
{
    Info = 0,
    Warning = 1,
    Downloading = 2,
    Delete = 3,
    Ready = 4
};*/

enum InstMod
{
    RussianFonts,
    CameraMod,
    GridHotkeys,
    TransparentCameraTrapezoid
};

enum WinCondition
{
    ANNIHILATE,
    ASSASSINATE,
    CONTROLAREA,
    DESTROYHQ,
    ECONOMICVICTORY,
    GAMETIMER,
    STRATEGICOBJECTIVE,
    SUDDENDEATH
};

enum FinalState
{
    unknown0 = 0,
    inGame = 1,
    unknown1 = 2,
    loser = 3,
    unknown2 = 4,
    winner = 5,

};

enum SsState
{
    ssShutdowned = 0,           //Сс выключен
    ssInitialized = 1,          //Сс загружен
};


enum SsMissionState
{
    unknown = 0,

    gameLoadStarted = 1,        //Начало загрузки Игры
    playbackLoadStarted = 2,    //Начало загрузки Реплея
    savedGameLoadStarted = 3,   //Начало загрузки сохраненной игры

    gameStarted = 4,            //Начало игры
    playbackStarted = 5,        //Начало реплея
    savedGameStarted = 6,       //Начало загруженной игры
    unknownGameStarted= 7,      //Начало хрен пойми какой игры (работает когда включили статку посреди игры)

    gameStoped = 8,             //Игра завершена
    playbackStoped = 9,         //Игра завершена
    savedGameStoped = 10,       //Игра завершена
    unknownGameStoped = 11,     //Игра завершена

    gameOver = 12,              //Победа какой-то команды в миссии
    playbackOver = 13,          //Победа какой-то команды в реплее
    savedGameOver = 14,         //Победа какой-то команды в сохраненной игре
    unknownGameOver = 15        //Победа какой-то команды в неизвестной игре
};

enum Race
{
    SpaceMarines = 0,
    ChaosMarines = 1,
    Orks = 2,
    Eldar = 3,
    ImperialGuard = 4,
    Necrons = 5,
    TauEmpire = 6,
    SistersOfBattle = 7,
    DarkEldar = 8
};

enum GameTypeForReplaySending
{
    GameType1x1 = 1,
    GameType2x2 = 2,
    GameType3x3 = 3,
    GameType4x4 = 4
};

struct PlayerData
{
    QString partyID = "";
    QString profileID = "";
    int position = 0;
};

struct PartyData
{
    QString partyId = "";
    QString hostPlayerId = "";
    QString partyMapName = "";
    QString partyName = "";
    QVector<PlayerData> profilesIds = {};
};

struct PlayerStats
{
    QString name = "";
    QString race = "";
    QString team = "";
    FinalState finalState;
    bool pHuman = true;
};

struct ServerPlayerStats
{
    QString steamId = "";
    int apm = 0;
    int gamesCount = 0;
    int mmr = 0;
    int mmr1v1 = 0;
    QString name = "";
    QString dowServerName = "";
    QString avatarUrl = "";
    int rank = 8;
    int race = 0;
    int winRate = 0;
    int winsCount = 0;
    bool isBanned = false;
    BanType::BanTypeEnum banType = BanType::NotBanned;
    QImage avatar;
    bool isCurrentPlayer = false;
    bool avatarAvailable = false;
    bool statsAvailable = false;
    int position = 0;
    int calibrateGamesLeft = 0;
};

struct PlayerInfoFromDowServer
{
    QString playerID;
    QString steamId;
    int position;
    QString name;
    bool isCurrentPlayer = false;
    bool closeConnection = false;
};

struct RankDiversion
{
    int rank7Min = 0;
    int rank7Max = 0;
    int rank6Min = 0;
    int rank6Max = 0;
    int rank5Min = 0;
    int rank5Max = 0;
    int rank4Min = 0;
    int rank4Max = 0;
    int rank3Min = 0;
    int rank3Max = 0;
    int rank2Min = 0;
    int rank2Max = 0;
    int rank1Min = 0;
    int rank1Max = 0;
};

struct UniqueOnlineStatistic
{
    int day = 0;
    int month = 0;
    int year = 0;
    int total = 0;
};

static const quint32 CRC32Table[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

struct PlayerInfoForReplaySending
{
    QString playerName;
    QString playerSid = "";
    Race playerRace;
    bool isWinner;
};

struct SendingReplayInfo
{
    QList<PlayerInfoForReplaySending> playersInfo;
    int apm;
    GameTypeForReplaySending gameType;
    QString mapName;
    int gameTime;
    QString mod;
    QString modVersion;
    WinCondition winBy;
    bool isFullStdGame = false;
};


struct DiscordMessage
{
    QString messageId;
    QString content;
    QString userId;
    QString userName;
    QDateTime timestamp;
    QString avatarId;
    QString avatarUrl;
    QString attachmentImageId;
    QString attachmentImageUrl;
    int attachmentImageWidth = 0;
    int attachmentImageHeight = 0;
    bool isNew = true;
    QString youtubeId;

   /* DiscordMessage& operator=(const DiscordMessage& message)
    {
        messageId = message.messageId;
        content = message.content;
        userId = message.userId;
        userName = message.userName;
        timestamp = message.timestamp;
        avatarId = message.avatarId;
        avatarUrl = message.avatarUrl;
        attachmentImageId = message.attachmentImageId;
        attachmentImageUrl = message.attachmentImageUrl;
        attachmentImageWidth = message.attachmentImageWidth;
        attachmentImageHeight = message.attachmentImageHeight;
        isNew = message.isNew;
        youtubeId = message.youtubeId;

         return *this;
    }*/
};

struct ReplayListInfo
{
    QString fileName;
    QString name;
    QString map;
    QString mapUrl;
    QString mod;
    QDateTime time;
    bool selected = false;
};

struct ReplayPlayer
{
    QString name;
    QString team;
    QString type;
    QString race;
    QString color;
    QString bannerUrl;
    QString badgeUrl;
};

enum SortType : int
{
    SortByTime = 0,
    SortByFileName = 1,
    SortByName = 2,
    SortByMod = 3,
    SortByMap = 4
};

struct PlyersRankedState
{
    QString name = "";
    QString steamId = "";
    bool isRanked = true;
    bool isOnline = false;
};

struct MapFileHash
{
    QString fileName = "";
    QString hash = "";
};

struct MapItem
{
    QString id;
    QString authors;
    QString description;
    QString webPage;
    QString mapName;
    QString modContentHash;
    long int unpackedSize;
    long int packedSize;
    int type;
    QList<QString> tags;

    QList<MapFileHash> filesList;

    bool needInstall = false;
    bool needUpdate = false;

    int downloadedFiles = 0;
    bool downloadProcessed = false;

    bool selected = false;
};

struct ModInfo{
    QString technicalName = "";
    QString uiName = "";
    QString version = "";
    QString changelog = "";
    bool isLatest = false;                  //Это последняя версия мода
    bool isInstalled = false;               //Этот мод установлен
    bool isCurrentMod = false;              //Это текущий мод в игре
    bool downloadingProcessed = false;      //Этот мод в процессе скачивания
    bool isPrevious = false;                //Этот мод был предыдущей версией мода
    bool isBeta = false;                    //Это бетта
    bool installingError = false;           //Ошибка загрузки/установки
};

enum LaunchMod{
    OriginalSoulstorm = 0,
    DowStatsBalanceMod = 1,
    LastSelectedMod = 2
};

struct NotificationInfo
{
    NotificationInfo& operator=(const NotificationInfo& copy)
    {
        if(&copy != this)
        {
            type = copy.type;
            text = copy.text;
            uuid = copy.uuid;
        }
        return *this;
    }

    NotificationType::NotificationTypeEnum type = NotificationType::Info;
    QString text = "";
    QUuid uuid = QUuid().createUuid();
};


#endif // BASETYPES_H
