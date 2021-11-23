#ifndef KEYBOARDPROCESSOR_H
#define KEYBOARDPROCESSOR_H

#include <QObject>
#include <QKeyEvent>

class KeyboardProcessor : public QObject
{
    Q_OBJECT
public:
    KeyboardProcessor(QObject* parent = nullptr);

    bool keyPressEvent(QKeyEvent event);

signals:
    void expandKeyPressed();

};

#endif // KEYBOARDPROCESSOR_H
