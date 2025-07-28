import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.0
import DowStatsStyle 1.0

Rectangle {
    id: root
    Layout.margins: 5

    Layout.minimumWidth: 60
    Layout.minimumHeight: 60

    height: colomnLayot.height

    property string userName: qsTr("Unknown user")
    property string content: qsTr("Message text")
    property string timesTamp: "00-00-0000 00:00"
    property string avatarId : ""
    property string attachmentId : ""
    property string youtubeId : ""
    property int attachmentImageWidth: 0
    property int attachmentImageHeight: 0
    property int youtubeImageWidth: 0
    property int youtubeImageHeight: 0
    property bool isNew: false

    property int attachmentImageScaledWidth: 0
    property int attachmentImageScaledHeight: 0

    function updateAttachmentImage(){
        if (root.attachmentImageHeight > root.attachmentImageWidth)
        {
            attachmentImageScaledHeight = 400;
            attachmentImageScaledWidth = (attachmentImageScaledHeight * root.attachmentImageWidth / root.attachmentImageHeight);
        }
        else
        {
            attachmentImageScaledWidth = 400;
            attachmentImageScaledHeight = (attachmentImageScaledWidth * root.attachmentImageHeight / root.attachmentImageWidth);
        }
    }

    color: DowStatsStyle.alternateBackgroundColor
    radius: 10
    border.color: "#00000000"

    signal attachmentImageClicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }

    ColumnLayout
    {
        width: parent.width
        id: colomnLayot
        spacing: 15

        RowLayout
        {
            id: messageHeader

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 15
            Layout.leftMargin: 15
            Layout.rightMargin: 15

            Rectangle{
                width: avatarRectangle.width + userNameLabel.width + 10 + 30
                height: avatarRectangle.height
                color: DowStatsStyle.itemColor

                radius: 10

                RowLayout{

                    Rectangle {
                        id: avatarRectangle

                        width: 50
                        height:50
                        radius: 10
                        Layout.rightMargin: 15

                        Image {
                            id: avatarImage
                            cache: false
                            anchors.fill: parent
                            source: "image://imageprovider/" + root.avatarId
                            visible: false
                        }

                         Rectangle {
                             id: maskRectangle

                             anchors.fill: parent
                             radius: 10
                             visible: false

                         }

                         OpacityMask {
                             id: opacityMask
                             anchors.fill: avatarImage
                             source: avatarImage
                             maskSource: maskRectangle
                         }
                    }

                    Label{
                        id: userNameLabel
                        Layout.rightMargin: 15
                        color: DowStatsStyle.textColor
                        text: root.userName
                        font.pixelSize: 18
                    }
                }
            }

            Label{
                id: newMessageLabel
                visible: root.isNew
                color: "#e60000"
                text: qsTr("NEW")
                font.pointSize: 10
                Layout.rightMargin: 15
            }

            Rectangle{
                Layout.fillWidth: true
            }

            Label{
                id: dateTimeLabel
                color: DowStatsStyle.textColor
                text: root.timesTamp
                font.pixelSize: 16
                Layout.rightMargin: 15
            }
        }

        TextArea{
            id:contentTextArea

            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.fillWidth: true

            text: root.content
            wrapMode: Text.Wrap
            color: DowStatsStyle.textColor
            font.pixelSize: 14
            textFormat: Text.RichText

            visible: !(root.content == "\0" || root.content == "")

            selectByMouse: true
            readOnly: true

            onLinkActivated: Qt.openUrlExternally(link)

            onLinkHovered:{

                if (hoveredLink != "")
                    mouseArea.cursorShape = Qt.PointingHandCursor;
                else
                    //mouseArea.cursorShape = Qt.IBeamCursor;
                    mouseArea.cursorShape = Qt.ArrowCursor;
            }
        }

        Rectangle {
            id: attachmentRectangle

            visible: root.attachmentId != "0"
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            radius: 10

            Layout.preferredWidth: root.attachmentImageScaledWidth
            Layout.preferredHeight: root.attachmentImageScaledHeight

            MouseArea
            {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    root.attachmentImageClicked();
                }
            }

            Image {
                id: attachmentImage
                cache: false
                anchors.fill: parent
                source: "image://imageprovider/" + root.attachmentId
                visible: false

                onSourceChanged:  root.updateAttachmentImage()
            }

            Rectangle {
                id: attachmentMaskRectangle
                anchors.fill: parent
                radius: 10
                visible: false
            }

            OpacityMask {
                id: attachmentOpacityMask
                anchors.fill: attachmentImage
                source: attachmentImage
                maskSource: attachmentMaskRectangle
            }
        }

        Rectangle
        {
           id: fullYoutubeRectangle
           visible: root.youtubeId != ""

           Layout.leftMargin: 15
           Layout.rightMargin: 15
           radius: 10


           color: "#ffffff"

           width: youtubeRectangle.width
           height: columnLayout3.height//youtubeRectangle.height + 40

           ColumnLayout
           {
               id: columnLayout3

               Image {
                   source: "qrc:/images/resources/images/youtube.svg"
                   Layout.preferredWidth: 100
                   Layout.preferredHeight: 40

                   Layout.topMargin: 7
                   Layout.leftMargin: 7
                   Layout.alignment: Qt.AlignVCenter

                   sourceSize.width: 100
                   sourceSize.height: 40

                   z: 1
               }

                Rectangle {
                    id: youtubeRectangle
                    radius: 10

                    Layout.alignment: Qt.AlignBottom

                    Image {
                        id: youtubeImage
                        cache: false
                        anchors.fill: parent
                        source: "image://imageprovider/" + root.youtubeId
                        visible: false

                        onSourceChanged: {
                            if (root.youtubeImageHeight > root.youtubeImageWidth)
                            {
                                youtubeRectangle.height = 400;
                                youtubeRectangle.width = (youtubeRectangle.height * root.youtubeImageWidth / root.youtubetImageHeight);
                            }
                            else
                            {
                                youtubeRectangle.width = 400;
                                youtubeRectangle.height = (youtubeRectangle.width * root.youtubeImageHeight / root.youtubeImageWidth);
                            }
                        }
                    }

                     Rectangle {
                         id: youtubeMaskRectangle
                         anchors.fill: parent
                         radius: 10
                         visible: false

                     }

                     OpacityMask {
                         id: youtubeOpacityMask
                         anchors.fill: youtubeImage
                         source: youtubeImage
                         maskSource: youtubeMaskRectangle
                     }
                }
            }

           Rectangle{

               Layout.fillHeight: true
           }

           MouseArea
           {
               anchors.fill: parent
               cursorShape: Qt.PointingHandCursor

               onClicked: {
                    Qt.openUrlExternally( "https://youtu.be/" + root.youtubeId);
               }
           }
        }

        Rectangle{
            id: bottomSpacerRectangle
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#00000000"
            border.color: "#00000000"
        }
    }
}



