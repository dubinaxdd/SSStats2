#ifndef EXTENDEDBINREADER_H
#define EXTENDEDBINREADER_H

#include <QString>
#include <QDataStream>
#include <QTextCodec>

class ExtendedBinReader : public QDataStream
{
public:

    ExtendedBinReader(QIODevice *parent = 0);
    ~ExtendedBinReader();

    bool seek(qint64 pos);
    qint64 pos();
    qint16 ReadInt16();
    qint32 ReadInt32();
    void WriteInt32(int num);

    char ReadChar();
    QString ReadStringUTF8(int count);
    QString ReadStringUTF16(int count);
    QByteArray ReadBytesArray(int count);
};

#endif // EXTENDEDBINREADER_H
