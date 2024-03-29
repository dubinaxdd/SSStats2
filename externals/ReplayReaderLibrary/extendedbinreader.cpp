#include <extendedbinreader.h>
#include <QDebug>
#include <typeinfo>

using namespace ReplayReader;
using namespace std;

ExtendedBinReader::ExtendedBinReader(QIODevice *parent)
    : QDataStream(parent)
{
    this->setByteOrder(QDataStream::LittleEndian);
}

ExtendedBinReader::~ExtendedBinReader()
{

}

bool ExtendedBinReader::seek(qint64 pos)
{
    return device()->seek(pos);
}

qint64 ExtendedBinReader::pos()
{
    return device()->pos();
}

qint16 ExtendedBinReader::ReadInt16()
{
    qint16 temp;
    *this >> temp;
    return temp;
}

qint32 ExtendedBinReader::ReadInt32()
{
    qint32 temp;
    *this >> temp;
    return temp;
}

qint64 ExtendedBinReader::ReadInt64()
{
    qint64 temp;
    *this >> temp;
    return temp;
}

char ExtendedBinReader::ReadChar()
{
    char temp;
    readRawData(&temp, 1);
    return temp;
}

QString ExtendedBinReader::ReadStringUTF8(int count )
{
    QVector<char> temp;
    temp.resize(count+1);

    //char temp[count];
    memset(temp.data(), 0, count+1);
    //temp[count]={0};
    temp[count]={0};
    readRawData(temp.data(), count);
    return QString::fromUtf8(temp.data());
}
QString ExtendedBinReader::ReadStringUTF16(int count)
{
    QVector<char> temp;
    temp.resize(count*2+2);

    //char temp[count*2+2];
    memset(temp.data(), 0, count*2+2);
    //temp[count*2+2]={0};

    temp[count*2 + 1]={0};
    readRawData(temp.data(), count*2);
    return QString::fromUtf16((ushort*)temp.data());
}

void ExtendedBinReader::WriteInt32(int num)
{
    *this << num;
}

// читает массив байт размером count из бинарного файла и возвращает его
QByteArray ExtendedBinReader::ReadBytesArray(int count)
{
    QVector<char> temp;
    temp.resize(count);

    //char temp[count];
    memset(temp.data(), 0, count);
    temp[count - 1] = {0};
    readRawData(temp.data(), count);
    return QByteArray(temp.data(), count);
}
