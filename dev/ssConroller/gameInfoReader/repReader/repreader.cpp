#include "repreader.h"
#include <QVector>
#include <QDebug>
#include "repreader.h"
#include <QFile>
#include <QStringList>
#include <QTextCodec>
#include <QDir>

RepReader::RepReader(QString fileName)
{
    pfile = new QFile(fileName);
    if(pfile->open(QIODevice::ReadWrite)){
        BinReader = new ExtendedBinReader(pfile);
        replay.setFileName(fileName);
        ReadReplayFully();
    }else
        qDebug() << "Could not open file" << fileName   ;
}

RepReader::~RepReader()
{
    if(BinReader) delete BinReader;
    close();
}

bool RepReader::ReadReplayFully()
{
    if (ReadHeader())
    {
        for (uint players = 0; players < replay.Slots; players++)
            replay.Players.append(ReadPlayer());
//        qDebug() << BinReader->pos();
//        ReadActionDetail();
        return true;
    }
    return false;
}

bool RepReader::ReadHeader()
{
    // " версия"
    replay.Version = BinReader->ReadInt32();
//    qDebug() << replay.Version;
    // пропускает количество симоволов и имя движка
    replay.MOD = BinReader->ReadStringUTF8(32);
//    qDebug() << replay.MOD;
    BinReader->skipRawData(57); // Relic Chunky
    /*qDebug() <<*/ BinReader->ReadStringUTF8(8); // DATADATA
    BinReader->skipRawData(12);
    //"длительность реплея в тиках системных часов"
    replay.TotalTicks = BinReader->ReadInt32();
    replay.Duration = replay.TotalTicks/8; // [c]
    BinReader->skipRawData(24); // 1706509
    /*qDebug() <<*/ BinReader->ReadStringUTF8(8); // FOLDINFO
    BinReader->skipRawData(4);
    replay.BeginFOLDINFO = BinReader->pos();
    replay.LengthFOLDINFO = BinReader->ReadInt32();
    BinReader->skipRawData(4); // 166857
    /*qDebug() <<*/  BinReader->ReadStringUTF8(8); // GameInfo
    BinReader->skipRawData(1);
    /*qDebug() <<*/ BinReader->ReadStringUTF8(8); // FOLDWMAN
    BinReader->skipRawData(4);
    replay.BeginFOLDWMAN = BinReader->pos();
    replay.LengthFOLDWMAN = BinReader->ReadInt32();
    BinReader->skipRawData(32);
    replay.PlayerCount = BinReader->ReadInt32();
    // размер карты в ячейках
    replay.MapSize = BinReader->ReadInt32();
    // имя файла локали в котором хранится ссылка на локализованное имя карты
    /*qDebug() <<*/ BinReader->ReadStringUTF8(BinReader->ReadInt32());
    // локализованное имя карты
    replay.MapLocale = BinReader->ReadStringUTF16(BinReader->ReadInt32());
//    qDebug() << replay.MapLocale;
    // CRC32 хеш сумма карты
    QString map_name = BinReader->ReadStringUTF8(BinReader->ReadInt32());
    replay.Map = map_name.right(map_name.size() - 18);
    replay.MapCRC32 = BinReader->ReadInt32();
//    qDebug() << replay.Map;

    // 12 байт неизвестных данных
    BinReader->skipRawData(12);
    /*qDebug() <<*/ BinReader->ReadStringUTF8(8);
    // "Версия chunk"
    BinReader->skipRawData(4);

    replay.BeginDATABASE = BinReader->pos();
    // "Размер chunk"
    replay.LengthDATABASE = BinReader->ReadInt32();

    BinReader->skipRawData(16);

    // "Размер слотов в игре. Всегда 8"
    replay.Slots = BinReader->ReadInt32();

    // сложность ИИ
    replay.settings.AIDiff = BinReader->ReadInt32();
    BinReader->skipRawData(4);
    // начальные ресуры
    replay.settings.StartRes = BinReader->ReadInt32();
    BinReader->skipRawData(4);
    // фиксированные команды
    replay.settings.LockTeams = BinReader->ReadInt32();
    BinReader->skipRawData(4);
    // читы
    replay.settings.CheatsON = BinReader->ReadInt32();
    BinReader->skipRawData(4);
    // начальные позиции
    replay.settings.StartLocation = BinReader->ReadInt32();
    BinReader->skipRawData(4);
    // скорость игры
    replay.settings.GameSpeed = BinReader->ReadInt32();
    BinReader->skipRawData(4);

    // шаринг ресурсов
    replay.settings.ResShare = BinReader->ReadInt32();
    /*qDebug() <<*/ BinReader->skipRawData(4);

    // скорость прироста ресурсов
    replay.settings.ResRate = BinReader->ReadInt32();
    /*qDebug() <<*/ BinReader->skipRawData(4);

    BinReader->skipRawData(1);
    replay.BeginNAME = (int)BinReader->pos();
    // "Имя в списке реплеев"
    int nameLength = BinReader->ReadInt32();

//    qDebug() << nameLength << BinReader->pos();
    if (nameLength <= 0 || nameLength > 48)
        qDebug() << "Could not read replay name";
    else
        replay.Name = BinReader->ReadStringUTF16(nameLength);
//    qDebug() << replay.Name;
    BinReader->skipRawData(4);
    // "Условия победы"
    auto count = BinReader->ReadInt32();
    QVector<int> win_conditions;
    for(int i=0; i<count; ++i)
        win_conditions.append(BinReader->ReadInt32());

    replay.hasAnnihilate = win_conditions.contains((int)Replay::Annihilate);
    replay.hasSuddenDeath = win_conditions.contains((int)Replay::SuddenDeath);
    replay.hasAssassinate = win_conditions.contains((int)Replay::Assassinate);
    replay.hasEconomicVictory = win_conditions.contains((int)Replay::EconomicVictory);
    replay.hasControlArea = win_conditions.contains((int)Replay::ControlArea);
    replay.hasDestroyHQ = win_conditions.contains((int)Replay::DestroyHQ);
    replay.hasTakeAndHold = win_conditions.contains((int)Replay::TakeAndHold);
    replay.hasGameTimer = win_conditions.contains((int)Replay::GameTimer);

    BinReader->skipRawData(5);

    replay.PlayerStart = BinReader->pos();
    return true;
}

QByteArray RepReader::getReplayData()
{
    pfile->seek(0);
    return pfile->readAll();
}

bool RepReader::isStandart(int game_type)
{
    return replay.isStandart(game_type);
}

Player *RepReader::ReadPlayer()
{
    Player *player = new Player();
    auto readerString = BinReader->ReadStringUTF8(8);
//    qDebug() << BinReader->pos();
    if (readerString == "FOLDGPLY")
    {
        BinReader->skipRawData(4);
        long pos = BinReader->pos();
        replay.FOLDGPLYz.insert(pos, BinReader->ReadInt32());
//        qDebug() << replay.FOLDGPLYz.keys().last();
        BinReader->skipRawData(16);
        pos = BinReader->pos();
        replay.DATAINFOz.insert(pos, BinReader->ReadInt32());

        BinReader->skipRawData(4);

        player->Name = BinReader->ReadStringUTF16(BinReader->ReadInt32());
        // "Тип игрока 0 Host/2 player/4 specc/7 empty/1,3,11 computer"
        player->Type = BinReader->ReadInt32();
        player->Team = BinReader->ReadInt32() + 1;
        player->Race = BinReader->ReadStringUTF8(BinReader->ReadInt32());

//        qDebug() << BinReader->pos();
        if(replay.getVersion()==9)
            BinReader->skipRawData(49);
        else
            BinReader->skipRawData(61);

//        replay.BeginPlayerDiffirences.append(BinReader->pos());

        // если игрок не обозреватель
        if(player->Type!=7)
        {
            readerString = BinReader->ReadStringUTF8(8);
            if (readerString == "FOLDTCUC")
            {
//                qDebug() << readerString  << BinReader->pos();
                BinReader->skipRawData(4);
                BinReader->skipRawData(BinReader->ReadInt32()+4);

                for (int bannerbadge = 0; bannerbadge < 2; bannerbadge++)
                {
                    readerString = BinReader->ReadStringUTF8(8);
                    if (readerString == "FOLDTCBD")
                    {
//                        qDebug() << readerString << BinReader->pos();
                        BinReader->skipRawData(4);
                        BinReader->skipRawData(BinReader->ReadInt32()+4);
//                        // "Высота бэйджа"
//                        int height = BinReader->ReadInt32();
//                        // "Ширина бэйджа"
//                        int width = BinReader->ReadInt32();
//                        BinReader->skipRawData(24);
//                        BinReader->skipRawData(height*width*4);
                    }
                    else if (readerString == "FOLDTCBN")
                    {
//                        qDebug() << readerString << BinReader->pos();
                        BinReader->skipRawData(4);
                        BinReader->skipRawData(BinReader->ReadInt32()+4);
//                        // "Ширина баннера"
//                        int width = BinReader->ReadInt32();
//                        // "Высота баннера"
//                        int height = BinReader->ReadInt32();
//                        BinReader->skipRawData(24);
//                        BinReader->skipRawData(height*width*4);
                    }
                    else
                    {
                        bannerbadge = 2;
                        BinReader->seek(BinReader->pos()-8);
                    }
                }
            }
            else
            {
                BinReader->seek(BinReader->pos()-8);
            }
        }
    }
    else
    {
        BinReader->seek(BinReader->pos()-8);
    }
//    qDebug() << player->Name;
    return player;
}

void RepReader::ReadActionDetail()
{
    BinReader->seek(replay.LengthFOLDINFO+170);
    replay.ActionStart = BinReader->pos();
    qDebug() << replay.ActionStart;
    while (BinReader->pos() < BinReader->device()->size())
    {
        int action1 = BinReader->ReadInt32();
        int action1Len = BinReader->ReadInt32();

        if (action1==1)
        {
            int player_id = 0;
            qDebug() << "MsgPos:" << BinReader->pos();
            int action2 = BinReader->ReadInt32();
            if (action2==1)
            {
                qDebug() << "Chat Message Type 1:" << BinReader->pos();
                BinReader->skipRawData(5);
                QString sender = BinReader->ReadStringUTF16(BinReader->ReadInt32());
                player_id = BinReader->ReadInt32();
                BinReader->skipRawData(8);
                QString msg = BinReader->ReadStringUTF16(BinReader->ReadInt32());

                qDebug() << QString("%1:%2").arg(_lastTick/8/60, 2).arg(_lastTick/8%60, 2)
                         << player_id << QString(sender+": "+msg);
            }
            else if (action2==action1Len)
            {
//                qDebug() << "Chat Message Type 2:" << BinReader->pos();
                BinReader->seek(BinReader->ReadInt32() * 4+16);
            }
            else
            {
                // скорей всего это сообщение о смерти игрока или его вылете
//                qDebug() << "Chat Message Type 3:" << BinReader->pos();
                auto tempValue = BinReader->ReadInt32();
                BinReader->ReadChar();
                player_id = BinReader->ReadInt32();
                BinReader->ReadStringUTF8(tempValue-4);
//                qDebug() << QString("%1:%2").arg(_lastTick/8/60, 2).arg(_lastTick/8%60, 2)
//                         << player_id;
            }
        }
        else if (action1 == 0 && action1Len > 17)
        {
//            qDebug() << "Read Action" << BinReader->pos();
            // всегда 80
            BinReader->skipRawData(1);

            _lastTick = BinReader->ReadInt32();

//            qDebug() << 0;
            // общее количество выполненных действий
            replay.ActionCount = BinReader->ReadInt32();
            // «Случайный» номер, сгенерированный во время игры, для расчета того, ударили ли снаряды и т.д.
            BinReader->ReadInt32();

            auto action_count = BinReader->ReadInt32();

//            qDebug() << 1 << action_count;
            while(--action_count!=-1)
            {
                BinReader->skipRawData(8);
//                qDebug() << 1.1;
                int temp_len = BinReader->ReadInt32();
                BinReader->skipRawData(1);
//                qDebug() << 1.2 << temp_len;
                int i = 0;
                while(i<temp_len)
                {
//                    qDebug() << 2;
                    int j = 0;
                    GameAction *currentAction = new GameAction();
                    currentAction->Tick = _lastTick;
//                    auto actionPos = BinReader->pos();
                    auto sub_action_len = BinReader->ReadInt16();
                    BinReader->ReadInt32();
                    j += 6;
//                    qDebug() << 3 << sub_action_len;
                    currentAction->Kind = BinReader->ReadInt32();
                    currentAction->KindValue = BinReader->ReadInt32();

                    BinReader->skipRawData(1);
                    j += 9;
                    qint16 pNum1 = BinReader->ReadInt16();
//                    qDebug() << 4 << currentAction->Kind << pNum1;
                    currentAction->PlayerNumber = pNum1 - (qint16)1000;
                    qint16 pNum2 = BinReader->ReadInt16();
//                    qDebug() << "Player Number" << currentAction->PlayerNumber  << pNum2;
//                    if(currentAction->PlayerNumber==0)
//                    {
//                        qDebug() << "SubActionPos:" << BinReader->pos();
//                        qDebug() << "Action:" << currentAction->Kind << currentAction->KindValue;
//                    }

                    currentAction->PlayerNumber2 = pNum2 - (qint16)1000;
                    qint16 pActionNum =  BinReader->ReadInt16();
//                    qDebug() << "Player Number2" << currentAction->PlayerNumber2 << pActionNum;


                    currentAction->PlayerActionNumber = pActionNum;

//                    qDebug() << "ActionData2" << 5;
                    j += 6 ;
                    currentAction->ActionData2 = BinReader->ReadBytesArray(sub_action_len - j);

                    i += sub_action_len;

                    replay.Actions.append(currentAction);
                }
            }
        }
        else
            BinReader->skipRawData(action1Len);
    }
}

bool RepReader::convertReplayToSteamVersion()
{
    if(replay.getVersion()==10)
        return true;

    QFileInfo replay_info(replay.fileName);
    QDir current_dir;
    QString home_dir = current_dir.absolutePath();
    current_dir.cd(replay_info.absolutePath());
    QFile::copy(replay.fileName, "replay_backup.rec");

    BinReader->seek(0);
    // запишем новую версию реплея
    BinReader->WriteInt32(10);
//    qDebug() << replay.BeginFOLDINFO << replay.LengthFOLDINFO << replay.FOLDGPLYz.size();
    BinReader->seek(replay.BeginFOLDINFO);
    // изменяем размера блока FOLDINFO
    BinReader->WriteInt32(replay.LengthFOLDINFO+12*replay.FOLDGPLYz.size());

    // выполняем проход по всем блокам FOLDGPLY
    for(int i=0; i<replay.FOLDGPLYz.size(); ++i)
    {
//        qDebug() << replay.FOLDGPLYz.keys().at(i);

        // изменяем размера блока FOLDGPLY
        BinReader->seek(replay.FOLDGPLYz.keys().at(i)+i*12);
//        qDebug() << replay.FOLDGPLYz.values().at(i);
        BinReader->WriteInt32(replay.FOLDGPLYz.values().at(i)+12);

        // изменяем размера блока DATAINFO
        BinReader->skipRawData(16);
        int LengthDATAINFO = replay.DATAINFOz.values().at(i);
//        qDebug() << LengthDATAINFO;
        BinReader->WriteInt32(LengthDATAINFO+12);

        int BeginDATAINFO = BinReader->pos();
        BinReader->seek(BeginDATAINFO+LengthDATAINFO-40);

        QByteArray temp_buffer = QByteArray(12, 0)+BinReader->device()->readAll();
        BinReader->seek(BeginDATAINFO+LengthDATAINFO-40);
        BinReader->device()->write(temp_buffer);
    }


    QFile::remove("replay_backup.rec");
    current_dir.cd(home_dir);
    return true;
}


bool RepReader::convertReplayTo1_2Version()
{
//    if(rep_reader.replay.getVersion()==9)
//        return true;
//    BinReader->seek(replay.LengthFOLDINFO+170);

    return true;
}


QString RepReader::RenameReplay()
{
    if(replay.Name.isEmpty())
       return QString();

    QString ingame_rep_name="";
    QStringList clan_tags;
    int p_count = replay.PlayerCount;
    clan_tags <<"Cg|"<<"AZ|"<<"sF|"<<"HG|"<<"RuW|"<<"SWS}{"<<"Sugar_"<<"[TOXiC]"<<"{KILLA}"<<"[SWS]";
    if(p_count==2)
    {
        for(int i=0; i<p_count; ++i)
        {
            QString name = replay.Players.at(i)->Name;
            foreach (QString tag, clan_tags) {
                if(name.contains(tag))
                    replay.Players[i]->Name = name.remove(tag);
            }
        }
        ingame_rep_name += replay.Players.at(0)->getVeryShortRaceName();
        ingame_rep_name += "v"+replay.Players.at(1)->getVeryShortRaceName() + "|";
        ingame_rep_name += replay.getShortMapName() + "|";
        ingame_rep_name += replay.Players.at(0)->Name.left(8);
        ingame_rep_name += "V"+replay.Players.at(1)->Name.left(8);
    }
    else
    {
        for(int i=0; i<p_count; ++i)
        {
            ingame_rep_name += replay.Players.at(i)->getVeryShortRaceName();
            if(i!=p_count-1) ingame_rep_name += "v";
            else ingame_rep_name += "|";
        }
        ingame_rep_name += replay.getShortMapName();
    }

    // найдем строку за которой следует имя реплейя в файле
    BinReader->seek(replay.BeginNAME);

    // получим длину названия реплея в игре
    int rep_name_length = BinReader->ReadInt32();

    // обновим размеры блоков данных в файле
    // посчитаем разницу между старым и новым называнием
    int bytesLengthDifference = (ingame_rep_name.size() - rep_name_length)*2;

    BinReader->seek(replay.BeginFOLDINFO);
    BinReader->WriteInt32(replay.LengthFOLDINFO+bytesLengthDifference);

    BinReader->seek(replay.BeginDATABASE);
    BinReader->WriteInt32(replay.LengthDATABASE+bytesLengthDifference);

    BinReader->seek(replay.BeginNAME+4+rep_name_length*2);
    QByteArray temp_buffer;
    temp_buffer = BinReader->device()->readAll();

    BinReader->seek(replay.BeginNAME);
    *BinReader << ingame_rep_name.size();
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    QTextEncoder *encoderWithoutBom = codec->makeEncoder( QTextCodec::IgnoreHeader );
    QByteArray tr = encoderWithoutBom->fromUnicode(ingame_rep_name)+temp_buffer;
    BinReader->device()->write(tr);

    QString rep_filename="",races="",players="";


    for(int i=0; i<p_count; ++i)
    {
        races += replay.Players.at(i)->getVeryShortRaceName();
        players += "#"+replay.Players.at(i)->Name;
    }
    if(p_count<8)
        rep_filename += QString::number(p_count)+races+"#"+replay.getShortMapName()+players;
    else
        rep_filename += QString::number(p_count)+races+"#"+replay.getShortMapName();


    rep_filename.replace(QRegExp("[^\\w\.#]"),"");
    // чтобы игра смогла запустить реплей, размер его имени не должен превышать 54 символов
    // 4 символа это расширение файла, 5 символов это разделитель + id игры, однако id постоянно растет
    // и не всегда будет занимать 4 символа
    // найдено решение, которое позволяет удалять id игры перед скачиванием файла
    // поэтому можно увеличить размер имени файла до 50 символов
    if(rep_filename.size()>50)
        rep_filename.remove(50, rep_filename.size()-50);
//    rep_filename.replace(QRegExp("[^\\w_~`!@#№$%^&\(\)\[\]\{\}\.,:;-+="),"");
    return rep_filename;
}

int RepReader::GetAverageAPM(int id)
{
    return replay.getPlayerApm(id);
}

void RepReader::close()
{
    if(pfile!=nullptr){
        pfile->close();
        delete pfile;
        pfile = nullptr;
    }
}

QString RepReader::getReplayMod(QString replay)
{
    QFile file(replay);
    QString modName;
    if(file.open(QIODevice::ReadOnly)){
        QDataStream out(&file);
        out.skipRawData(4);
        char temp[33]={0};
        out.readRawData(temp, 32);
        modName = QString::fromUtf8(temp);
        file.close();
    }
    return modName;
}

QString RepReader::getReplayMap(QString replay)
{
    QFile file(replay);
    QString mapName;
    if(file.open(QIODevice::ReadOnly)){
        QDataStream out(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        out.skipRawData(226);
        qint32 count;
        out >> count;
        out.skipRawData(count);
        out >> count;
//        qDebug() << count << out.device()->pos();
        char temp[count*2+2];
        memset(temp, 0, count*2+2);
        temp[count*2+2]={0};
        out.readRawData(temp, count*2);
        mapName = QString::fromUtf16((ushort*)temp);
        file.close();
    }
    return mapName;
}

// был случай, когда игрок находился в обсах, а другой игрок, с таким же ником был в слоте игрока
// и тогда игрок из обса отправлял статистику, так как программа считала, что обс был в игре
// поэтому решено проверять находился ли игрок с таким же ником в обсах
bool RepReader::playerIsObserver(QString name)
{
    for(int i=0; i<replay.PlayerCount; ++i)
        if(replay.Players.at(i)->Name==name&&replay.Players.at(i)->Type==4)
            return true;
    return false;
}
