#include "keyboardprocessor.h"
#include <QDebug>

KeyboardProcessor::KeyboardProcessor(QObject* parent)
    : QObject(parent)
{

}

bool KeyboardProcessor::keyPressEvent(QKeyEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        if(event->key() == 192)
        {
            emit expandKeyPressed();
            return true;
        }
    }
    QObject::event(event);
    return false;
}

