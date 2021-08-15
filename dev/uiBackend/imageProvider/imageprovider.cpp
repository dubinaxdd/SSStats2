#include "imageprovider.h"
#include "QDebug"

ImageProvider::ImageProvider(QObject *parent)
    : QObject(parent)
    , QQuickImageProvider(QQmlImageProviderBase::ImageType::Image)
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)

    if(id == "currentPlayerAvatarMedium" && !m_currentPlayerAvatarMedium.isNull())
        return m_currentPlayerAvatarMedium;

    //если ничего не нашлось возвращаем черную картинку
    QImage image(60,60, QImage::Format_ARGB32);
    image.fill(QColor(0,0,0).rgba());
    return image;
}

void ImageProvider::setCurrentPlayerAvatarMedium(QImage newCurrentPlayerAvatarMedium)
{
    m_currentPlayerAvatarMedium = newCurrentPlayerAvatarMedium;
}
