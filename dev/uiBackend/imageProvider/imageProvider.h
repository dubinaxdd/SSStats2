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

    void setCurrentMiniMap(QImage currentMiniMap);

signals:
    void updateAvatars();
    void updateAttachments();
    void updateBanners();
    void updateYoutubeImages();
    void updateMiniMapImage();

public slots:
    void addDiscordAvatar(QString avatarId, QImage discordAvatar);
    //void updateStatsAvatar(QMap <QString, QImage> playerStatsAvatars);
    void addPlayerAvatar(QString avatarId, QImage image);

    void addAttachmentImage(QString attachmentId, QImage image);
    void addYoutubeImage(QString youtubeId, QImage image);
    void addReplayPlayerBanner(QString attachmentId, QImage image);

    void receiveMapImage(QImage mapImage, QString mapImageId);

private:
    QImage m_currentPlayerAvatarMedium;

    QMap <QString, QImage> m_playerStatsAvatars;
    QMap <QString, QImage> m_discordAvatars;
    QMap <QString, QImage> m_attachmentImages;
    QMap <QString, QImage> m_youtubeImages;
    QMap <QString, QImage> m_replayPlayerBanners;
    QMap <QString, QImage> m_miniMapImages;

    QImage m_currentMiniMap;
};

#endif // IMAGEPROVIDER_H
