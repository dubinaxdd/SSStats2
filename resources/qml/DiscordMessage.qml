import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1

Rectangle {
    id: mainRectangle
    Layout.margins: 5

    Layout.minimumWidth: 60
    Layout.minimumHeight: 60


    height: 100 + contentTextArea.height


    property url avatarSource
    property string userName: "Unknown user"
    property string content: "Message text"
    property string timesTamp: "00-00-0000 00:00"
    color: "#eaeaea"
    radius: 10
    border.color: "#00000000"

    ColumnLayout
    {
        anchors.fill: parent


        RowLayout
        {
            Layout.fillWidth: true

            Image {
                id: avatarImage
                source: mainRectangle.avatarSource

                width: 50
                height:50



                Layout.margins: 15
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


