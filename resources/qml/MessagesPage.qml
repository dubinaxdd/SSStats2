import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import DowStatsStyle 1.0

Rectangle {
    id: infoRectangle
    opacity: 1
    color: DowStatsStyle.backgroundColor
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    radius: 10

    property bool readyToRequestNextMessages: false

    onVisibleChanged: {
        if (infoRectangle.visible == false)
        {
            model.messagesReaded();
        }
    }

    property var model

    onWidthChanged: resizeFullAttachmentImage();
    onHeightChanged: resizeFullAttachmentImage();

    function resizeFullAttachmentImage(){
        if (fullAttachmenImageRectangle.attachmentImageHeight > fullAttachmenImageRectangle.attachmentImageWidth)
        {
            attachmentRectangle.height = fullAttachmenImageRectangle.height - 40;
            attachmentRectangle.width = (attachmentRectangle.height * fullAttachmenImageRectangle.attachmentImageWidth / fullAttachmenImageRectangle.attachmentImageHeight);

            if (attachmentRectangle.width > fullAttachmenImageRectangle.height - 40)
            {
                attachmentRectangle.width = fullAttachmenImageRectangle.width - 40;
                attachmentRectangle.height = (attachmentRectangle.width * fullAttachmenImageRectangle.attachmentImageHeight / fullAttachmenImageRectangle.attachmentImageWidth);
            }
        }
        else
        {
            attachmentRectangle.width = fullAttachmenImageRectangle.width - 40;
            attachmentRectangle.height = (attachmentRectangle.width * fullAttachmenImageRectangle.attachmentImageHeight / fullAttachmenImageRectangle.attachmentImageWidth);

            if (attachmentRectangle.height > fullAttachmenImageRectangle.height - 40)
            {
                attachmentRectangle.height = fullAttachmenImageRectangle.height - 40;
                attachmentRectangle.width = (attachmentRectangle.height * fullAttachmenImageRectangle.attachmentImageWidth / fullAttachmenImageRectangle.attachmentImageHeight);
            }
        }
    }


    Rectangle{

        id: fullAttachmenImageRectangle

        anchors.fill: parent
        visible: false
        color: "#cf1a1919"
        radius: 10

        z: 10

        property string attachmentId
        property int attachmentImageWidth
        property int attachmentImageHeight


        MouseArea {
            anchors.fill: parent

            cursorShape: Qt.PointingHandCursor

            onClicked: {
                fullAttachmenImageRectangle.visible = false;
            }
        }

        Rectangle {
            id: attachmentRectangle
            width: 0
            height: 0
            anchors.horizontalCenter: fullAttachmenImageRectangle.horizontalCenter
            anchors.verticalCenter: fullAttachmenImageRectangle.verticalCenter

            Image {
                id: attachmentImage
                cache: false
                anchors.fill: parent
                source: "image://imageprovider/" + fullAttachmenImageRectangle.attachmentId

                onSourceChanged: {
                    resizeFullAttachmentImage()
                }
            }
        }
    }


    ColumnLayout
    {
        anchors.fill: parent

        ListView{
            id: messagesListView
            Layout.margins: 10
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 5

            model: infoRectangle.model

            delegate: DiscordMessage{
                width: messagesListView.width - (scrollBar.visible ?  scrollBar.width + 5 : 0)

                userName: model.userName
                content: model.content
                timesTamp: model.timesTamp
                avatarId: model.avatarId
                attachmentId: model.attachmentId
                attachmentImageWidth: model.attachmentImageWidth
                attachmentImageHeight: model.attachmentImageHeight
                isNew: model.isNew
                youtubeId: model.youtubeId
                youtubeImageWidth: model.youtubeImageWidth
                youtubeImageHeight: model.youtubeImageHeight

                onAttachmentImageClicked: {
                    fullAttachmenImageRectangle.visible = true;
                    fullAttachmenImageRectangle.attachmentImageWidth = model.attachmentImageWidth;
                    fullAttachmenImageRectangle.attachmentImageHeight = model.attachmentImageHeight;
                    fullAttachmenImageRectangle.attachmentId = model.attachmentId;
                }
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
                visible: messagesListView.contentItem.height > messagesListView.height
                policy: ScrollBar.AlwaysOn
            }

            onAtYEndChanged: {
                if (atYEnd && readyToRequestNextMessages)
                {
                    model.requestNextMessages();
                }

                readyToRequestNextMessages = true;
            }
        }
    }
}
