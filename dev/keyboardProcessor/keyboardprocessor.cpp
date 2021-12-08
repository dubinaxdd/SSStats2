#include "keyboardprocessor.h"
#include <QDebug>

KeyboardProcessor::KeyboardProcessor(QObject* parent)
    : QObject(parent)
{

}

bool KeyboardProcessor::keyPressEvent(QKeyEvent event)
{
    if (event.type() == QEvent::KeyPress)
    {
        if(event.key() == 192 && event.modifiers() == Qt::KeyboardModifier::AltModifier)
        {
            emit expandKeyPressed();
            return true;
        }

        if(event.key() == 9 && event.modifiers() == Qt::KeyboardModifier::AltModifier)
        {
            emit altTabPressed();
            return true;
        }
    }
    return false;
}

