#include <imageProvider.h>
#include <QDebug>

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

    if (m_discordAvatars.contains(id))
        return m_discordAvatars.value(id);

    if (m_attachmentImages.contains(id))
        return m_attachmentImages.value(id);

    if (m_playerStatsAvatars.contains(id) && !m_playerStatsAvatars.value(id).isNull())
        return m_playerStatsAvatars.value(id);

    if (m_replayPlayerBanners.contains(id) && !m_replayPlayerBanners.value(id).isNull())
        return m_replayPlayerBanners.value(id);

    //если ничего не нашлось возвращаем черную картинку
    QImage image(60,60, QImage::Format_ARGB32);
    image.fill(QColor(0,0,0).rgba());
    return image;
}

void ImageProvider::setCurrentPlayerAvatarMedium(QImage newCurrentPlayerAvatarMedium)
{
    m_currentPlayerAvatarMedium = newCurrentPlayerAvatarMedium;
}

void ImageProvider::addDiscordAvatar(QString avatarId, QImage discordAvatar)
{
    m_discordAvatars.insert(avatarId, discordAvatar);
    emit updateAvatars();
}

void ImageProvider::addPlayerAvatar(QString avatarId, QImage image)
{
    m_attachmentImages.insert(avatarId, image);
}

void ImageProvider::addAttachmentImage(QString attachmentId, QImage image)
{
    m_attachmentImages.insert(attachmentId, image);
    emit updateAttachments();
}

void ImageProvider::addReplayPlayerBanner(QString attachmentId, QImage image)
{
    m_replayPlayerBanners.insert(attachmentId, image);
    emit updateBanners();
}



