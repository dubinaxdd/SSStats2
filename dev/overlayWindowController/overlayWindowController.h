#ifndef OVERLAYWINDOWCONTROLLER_H
#define OVERLAYWINDOWCONTROLLER_H

#include <QObject>

class OverlayWindowController : public QObject
{
    Q_OBJECT
public:
    explicit OverlayWindowController(QObject *parent = nullptr);

signals:

};

#endif // OVERLAYWINDOWCONTROLLER_H
