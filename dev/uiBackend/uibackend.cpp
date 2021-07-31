#include "uibackend.h"

UiBackend::UiBackend(QObject *parent) : QObject(parent)
{

}

void UiBackend::expandKeyPressed()
{
    setExpand(!m_expand);
}

void UiBackend::receiveSsMaximized(bool maximized)
{
    m_ssMaximized = maximized;
    setExpand(false);
    showClient();
}

void UiBackend::receiveSsLounched(bool lounched)
{
    m_ssLounched = lounched;
    setExpand(false);
    showClient();
}

void UiBackend::showClient()
{
    m_showClient = m_ssLounched && m_ssMaximized;
    emit sendShowClient(m_showClient);
}

bool UiBackend::expand() const
{
    return m_expand;
}

bool UiBackend::getShowClient()
{
    return m_showClient;
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

void UiBackend::setWindowTopmost(bool topmost)
{
    m_windowTopmost = topmost;
    emit windowTopmostChanged();
}

bool UiBackend::getWindowTopmost()
{
    return m_windowTopmost;
}

void UiBackend::setWindowedMode()
{
    emit windowedModeSeted();
}
