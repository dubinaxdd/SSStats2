#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>

class ImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = nullptr);

    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override;

    void setCurrentPlayerAvatarMedium(QImage newCurrentPlayerAvatarMedium);

signals:

private:
    QImage m_currentPlayerAvatarMedium;
};

#endif // IMAGEPROVIDER_H
