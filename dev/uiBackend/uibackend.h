#ifndef UIBACKEND_H
#define UIBACKEND_H

#include <QObject>
#include <QPoint>

class UiBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool expand MEMBER m_expand NOTIFY sendExpand)
    Q_PROPERTY(int mousePositionX READ mousePositionX)
    Q_PROPERTY(int mousePositionY READ mousePositionY)

public:
    explicit UiBackend(QObject *parent = nullptr);

    bool expand() const;
    void setExpand(bool newExpand);
    void mousePressEvent (QPoint mousePosition);
    int mousePositionX();
    int mousePositionY();

signals:
    void sendExpand(bool);
    void sendMousePress();

public slots:
    void expandKeyPressed();

private:
    bool m_expand = false;
    QPoint m_mousePosition;

};

#endif // UIBACKEND_H
