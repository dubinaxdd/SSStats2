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

    height: bottomSpacerRectangle.height + messageHeader.height + contentTextArea.height + 30

    property string userName: "Unknown user"
    property string content: "Message text"
    property string timesTamp: "00-00-0000 00:00"
    property string avatarId : ""
    color: "#eaeaea"
    radius: 10
    border.color: "#00000000"

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
            Rectangle{
                width: imageRectangle.width + userNameLabel.width + 10 + 30
                height: imageRectangle.height
                color: "#c8c8c8"

                radius: 10

                RowLayout{

                    Rectangle {
                        id: imageRectangle

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
                        font.pointSize: 14
                    }
                }
            }



            Rectangle{
                Layout.fillWidth: true
            }

            Label{
                id: dateTimeLabel
                color: "#26282a"
                text: mainRectangle.timesTamp
                font.pointSize: 10
                Layout.rightMargin: 15
            }
        }

        Text{
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.topMargin: 15
            Layout.fillWidth: true

            id:contentTextArea
            text: mainRectangle.content
            wrapMode: Text.Wrap
            color: "#26282a"
            font.pointSize: 10
            textFormat: Text.AutoText
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



