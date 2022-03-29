#include <imageprovider.h>
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

    if(id == "player2AvatarMedium" && !m_player2AvatarMedium.isNull())
        return m_player2AvatarMedium;

    if(id == "player3AvatarMedium" && !m_player3AvatarMedium.isNull())
        return m_player3AvatarMedium;

    if(id == "player4AvatarMedium" && !m_player4AvatarMedium.isNull())
        return m_player4AvatarMedium;

    if(id == "player5AvatarMedium" && !m_player5AvatarMedium.isNull())
        return m_player5AvatarMedium;

    if(id == "player6AvatarMedium" && !m_player6AvatarMedium.isNull())
        return m_player6AvatarMedium;

    if(id == "player7AvatarMedium" && !m_player7AvatarMedium.isNull())
        return m_player7AvatarMedium;

    if(id == "player8AvatarMedium" && !m_player8AvatarMedium.isNull())
        return m_player8AvatarMedium;


    for(int i = 0; i < m_discordAvatars.count(); i++)
    {
        if (id == m_discordAvatars.keys().at(i))
            return m_discordAvatars.values().at(i);

    }

    for(int i = 0; i < m_attachmentImages.count(); i++)
    {
        if (id == m_attachmentImages.keys().at(i))
            return m_attachmentImages.values().at(i);

    }

    //если ничего не нашлось возвращаем черную картинку
    QImage image(60,60, QImage::Format_ARGB32);
    image.fill(QColor(0,0,0).rgba());
    return image;
}

void ImageProvider::setCurrentPlayerAvatarMedium(QImage newCurrentPlayerAvatarMedium)
{
    m_currentPlayerAvatarMedium = newCurrentPlayerAvatarMedium;
}


void ImageProvider::setPlayer2AvatarMedium(QImage avatarMedium)
{
    m_player2AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer3AvatarMedium(QImage avatarMedium)
{
    m_player3AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer4AvatarMedium(QImage avatarMedium)
{
    m_player4AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer5AvatarMedium(QImage avatarMedium)
{
    m_player5AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer6AvatarMedium(QImage avatarMedium)
{
    m_player6AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer7AvatarMedium(QImage avatarMedium)
{
    m_player7AvatarMedium = avatarMedium;
}

void ImageProvider::setPlayer8AvatarMedium(QImage avatarMedium)
{
    m_player8AvatarMedium = avatarMedium;
}

void ImageProvider::addDiscordAvatar(QString avatarId, QImage discordAvatar)
{
    m_discordAvatars.insert(avatarId, discordAvatar);
    emit updateAvatars();
}

void ImageProvider::addAttachmentImage(QString attachmentId, QImage image)
{
    m_attachmentImages.insert(attachmentId, image);
    emit updateAttachments();
}



