import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.0
import DowStatsStyle 1.0

Rectangle {
    id: mainRectangle
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
                            source: "image://imageprovider/" + mainRectangle.avatarId
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
                        text: mainRectangle.userName
                        font.pixelSize: 18
                    }
                }
            }

            Label{
                id: newMessageLabel
                visible: mainRectangle.isNew
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
                text: mainRectangle.timesTamp
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

            text: mainRectangle.content
            wrapMode: Text.Wrap
            color: DowStatsStyle.textColor
            font.pixelSize: 14
            textFormat: Text.RichText

            visible: !(mainRectangle.content == "\0" || mainRectangle.content == "")

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

            visible: mainRectangle.attachmentId != "0"
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            radius: 10

            MouseArea
            {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    mainRectangle.attachmentImageClicked();
                }
            }

            Image {
                id: attachmentImage
                cache: false
                anchors.fill: parent
                source: "image://imageprovider/" + mainRectangle.attachmentId
                visible: false

                onSourceChanged: {
                    if (mainRectangle.attachmentImageHeight > mainRectangle.attachmentImageWidth)
                    {
                        attachmentRectangle.height = 400;
                        attachmentRectangle.width = (attachmentRectangle.height * mainRectangle.attachmentImageWidth / mainRectangle.attachmentImageHeight);
                    }
                    else
                    {
                        attachmentRectangle.width = 400;
                        attachmentRectangle.height = (attachmentRectangle.width * mainRectangle.attachmentImageHeight / mainRectangle.attachmentImageWidth);
                    }
                }
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
           visible: mainRectangle.youtubeId != ""

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
                        source: "image://imageprovider/" + mainRectangle.youtubeId
                        visible: false

                        onSourceChanged: {
                            if (mainRectangle.youtubeImageHeight > mainRectangle.youtubeImageWidth)
                            {
                                youtubeRectangle.height = 400;
                                youtubeRectangle.width = (youtubeRectangle.height * mainRectangle.youtubeImageWidth / mainRectangle.youtubetImageHeight);
                            }
                            else
                            {
                                youtubeRectangle.width = 400;
                                youtubeRectangle.height = (youtubeRectangle.width * mainRectangle.youtubeImageHeight / mainRectangle.youtubeImageWidth);
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
                    Qt.openUrlExternally( "https://youtu.be/" + mainRectangle.youtubeId);
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



