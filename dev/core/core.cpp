#include "core.h"
#include "QDebug"


Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_uiBackend(new UiBackend(context))
    , m_ssController(new SsController(this))
{
    context->setContextProperty("_uiBackend", m_uiBackend);
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_uiBackend, &UiBackend::sendExpand, m_ssController, &SsController::blockInput, Qt::QueuedConnection );
    QObject::connect(m_ssController, &SsController::ssLounched, m_uiBackend, &UiBackend::receiveSsLounched, Qt::QueuedConnection );
    QObject::connect(m_ssController, &SsController::ssMaximized, m_uiBackend, &UiBackend::receiveSsMaximized, Qt::QueuedConnection );

}


void Core::topmostTimerTimout()
{
    //TODO
    //Костыль для того чтоб форма всегда была с верху сса,
    //ибо директикс сса по таймеру обновляет флаги стиля окна, конкретнро WS_EX_TOPMOST
    //установка флага Qt::WindowStaysOnTopHint на кородкое время выводет икно повер сса
    //Соответственно затираем флаг и выставлем заного по таймеру.
    //Время устанавливаемое таймеру возможно придется менять из за разницы систем, надо тестить
    if (m_ssStatsHwnd){
        BringWindowToTop(m_ssStatsHwnd);
    }
}

UiBackend *Core::uiBackend() const
{
    return m_uiBackend;
}

SsController *Core::ssController() const
{
    return m_ssController;
}

bool Core::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress && m_keyboardProcessor)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        m_keyboardProcessor->keyPressEvent(keyEvent);
        return true;
    }

     if (event->type() == QEvent::MouseButtonPress)
     {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         m_uiBackend->mousePressEvent(mouseEvent->pos());
         return true;
     }

    QObject::event(event);
    return false;

}

void Core::startTopmost()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("SSStats2");
    LPCWSTR lps = (LPCWSTR)s.utf16();

    m_ssStatsHwnd = FindWindowW(NULL, lps);

    m_topmostTimer = new QTimer();
    m_topmostTimer->setInterval(100);
    connect(m_topmostTimer, &QTimer::timeout, this, &Core::topmostTimerTimout, Qt::QueuedConnection);
    m_topmostTimer->start();
}
