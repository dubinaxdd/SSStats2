#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>

class UiBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool expand MEMBER m_expand NOTIFY sendExpand)
    Q_PROPERTY(bool showClient MEMBER m_showClient NOTIFY sendShowClient)
    Q_PROPERTY(int mousePositionX READ mousePositionX)
    Q_PROPERTY(int mousePositionY READ mousePositionY)
    Q_PROPERTY(bool topmost READ getWindowTopmost WRITE setWindowTopmost)
    Q_PROPERTY(bool gamePanelVisible MEMBER m_gamePanelVisible NOTIFY gamePanelVisibleChanged)


public:
    explicit UiBackend(QObject *parent = nullptr);

    bool expand() const;
    bool getShowClient();
    void setExpand(bool newExpand);
    void mousePressEvent (QPoint mousePosition);
    int mousePositionX();
    int mousePositionY();
    void setWindowTopmost(bool topmost);
    bool getWindowTopmost();
    void setWindowedMode();

signals:
    void sendExpand(bool);
    void sendMousePress();
    void sendShowClient(bool);
    void windowTopmostChanged();
    void windowedModeSeted();
    void gamePanelVisibleChanged(bool);

public slots:
    void expandKeyPressed();
    void receiveSsMaximized(bool maximized);
    void receiveSsLounched(bool lounched);

    void gameStarted();
    void gameStoped();


private:
    void showClient();

private:
    bool m_expand = false;
    QPoint m_mousePosition;
    bool m_ssMaximized = false;
    bool m_ssLounched = false;
    bool m_showClient = false;
    bool m_gamePanelVisible = false;

    bool m_windowTopmost = false;
};

#endif // UIBACKEND_H
