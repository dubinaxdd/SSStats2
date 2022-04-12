import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import QtGraphicalEffects 1.0

Rectangle {
    id: mainRectangle
    Layout.margins: 5

    Layout.minimumWidth: 60
    Layout.minimumHeight: 60

    height: 15 + messageHeader.height
               + (contentTextArea.visible ? 15 : 0)
               + (contentTextArea.visible ? contentTextArea.height : 0)
               + (attachmentRectangle.visible ? 15 : 0)
               + (attachmentRectangle.visible ? attachmentRectangle.height : 0)
               + bottomSpacerRectangle.height

    property string userName: "Unknown user"
    property string content: "Message text"
    property string timesTamp: "00-00-0000 00:00"
    property string avatarId : ""
    property string attachmentId : ""
    property int attachmentImageWidth: 0
    property int attachmentImageHeight: 0
    property bool isNew: false

    color: "#eaeaea"
    radius: 10
    border.color: "#00000000"

    signal attachmentImageClicked()

    MouseArea {
        id: mouseArea
        anchors.fill: parent
/*
        hoverEnabled: true

        onHoveredChanged: {
            mouseArea.cursorShape = Qt.IBeamCursor;
        }*/
    }

    ColumnLayout
    {
        anchors.fill: parent

        spacing: 0

        RowLayout
        {
            id: messageHeader

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 15
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.bottomMargin: contentTextArea.visible ? 15 : 0

            Rectangle{
                width: avatarRectangle.width + userNameLabel.width + 10 + 30
                height: avatarRectangle.height
                color: "#c8c8c8"

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
                            source: "image://ImageProvider/" + mainRectangle.avatarId
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
                        color: "#26282a"
                        text: mainRectangle.userName
                        font.pixelSize: 22
                    }
                }
            }

            Label{
                id: newMessageLabel
                visible: mainRectangle.isNew
                color: "#e60000"
                text: "NEW"
                font.pointSize: 10
                Layout.rightMargin: 15
            }

            Rectangle{
                Layout.fillWidth: true
            }

            Label{
                id: dateTimeLabel
                color: "#26282a"
                text: mainRectangle.timesTamp
                font.pixelSize: 18
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
            color: "#26282a"
            font.pixelSize: 16
            textFormat: Text.RichText

            visible: mainRectangle.content != "\0"

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
            Layout.topMargin: contentTextArea.visible ? 15 : 0

            radius: 10
            Layout.rightMargin: 15

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
                source: "image://ImageProvider/" + mainRectangle.attachmentId
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

        Rectangle{
            id: bottomSpacerRectangle
            Layout.fillWidth: true
            height: 15
            color: "#00000000"
            border.color: "#00000000"
        }
    }
}



