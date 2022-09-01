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

    void setPlayer2AvatarMedium(QImage avatarMedium);
    void setPlayer3AvatarMedium(QImage avatarMedium);
    void setPlayer4AvatarMedium(QImage avatarMedium);
    void setPlayer5AvatarMedium(QImage avatarMedium);
    void setPlayer6AvatarMedium(QImage avatarMedium);
    void setPlayer7AvatarMedium(QImage avatarMedium);
    void setPlayer8AvatarMedium(QImage avatarMedium);

signals:
    void updateAvatars();
    void updateAttachments();

public slots:
    void addDiscordAvatar(QString avatarId, QImage discordAvatar);
    //void updateStatsAvatar(QMap <QString, QImage> playerStatsAvatars);
    void addPlayerAvatar(QString avatarId, QImage image);

    void addAttachmentImage(QString attachmentId, QImage image);

private:
    QImage m_currentPlayerAvatarMedium;

    QMap <QString, QImage> m_playerStatsAvatars;
    QMap <QString, QImage> m_discordAvatars;
    QMap <QString, QImage> m_attachmentImages;
};

#endif // IMAGEPROVIDER_H