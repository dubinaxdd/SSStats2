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


    height: 110 + contentTextArea.height


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

        RowLayout
        {
            Layout.fillWidth: true

            Rectangle {
                id: imageRectangle

                width: 65
                height:65

                radius: 10

                Layout.margins: 15

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

                Layout.margins: 15
            }
        }



        TextArea{

            Layout.minimumWidth: 40

            Layout.fillWidth: true
            //Layout.fillHeight: true

            id:contentTextArea
            text: mainRectangle.content
            wrapMode: Text.Wrap
            placeholderTextColor: "#fcfdfe"
            font.pointSize: 9
            textFormat: Text.AutoText
        }
    }
}



