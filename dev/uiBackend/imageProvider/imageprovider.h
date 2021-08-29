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

private:
    QImage m_currentPlayerAvatarMedium;

    QImage m_player2AvatarMedium;
    QImage m_player3AvatarMedium;
    QImage m_player4AvatarMedium;
    QImage m_player5AvatarMedium;
    QImage m_player6AvatarMedium;
    QImage m_player7AvatarMedium;
    QImage m_player8AvatarMedium;
};

#endif // IMAGEPROVIDER_H
