#include "uibackend.h"

UiBackend::UiBackend(QObject *parent) : QObject(parent)
{

}

void UiBackend::expandKeyPressed()
{
    setExpand(!m_expand);
}

void UiBackend::showClient(bool show)
{
    m_showClient = show;
    emit sendShowClient(m_showClient);
}

bool UiBackend::expand() const
{
    return m_expand;
}

void UiBackend::setExpand(bool newExpand)
{
    m_expand = newExpand;
    emit sendExpand(m_expand);
}

void UiBackend::mousePressEvent(QPoint mousePosition)
{
    m_mousePosition = mousePosition;
    emit sendMousePress();
}

int UiBackend::mousePositionX()
{
    return m_mousePosition.x();
}

int UiBackend::mousePositionY()
{
    return m_mousePosition.y();
}
