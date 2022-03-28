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


    height: 120 + contentTextArea.height


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
            Layout.fillWidth: true

            Rectangle {
                id: imageRectangle

                width: 55
                height:55

                radius: 10

                Layout.leftMargin: 15
                Layout.rightMargin: 15
                //Layout.topMargin: 15
                //Layout.bottomMargin: 5

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
                color: "#26282a"
                text: mainRectangle.userName
                font.pointSize: 14
            }

            Rectangle{
                Layout.fillWidth: true
            }

            Label{
                id: dateTimeLabel
                color: "#26282a"
                text: mainRectangle.timesTamp

                Layout.rightMargin: 15
            }
        }

        Text{

            Layout.minimumWidth: 40
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Layout.fillWidth: true
            id:contentTextArea
            text: mainRectangle.content
            wrapMode: Text.Wrap
            color: "#26282a"
            font.pointSize: 11
            textFormat: Text.AutoText
        }

        Rectangle{
            Layout.fillWidth: true
            height: 10
            color: "#00000000"
            border.color: "#00000000"
        }
    }
}



