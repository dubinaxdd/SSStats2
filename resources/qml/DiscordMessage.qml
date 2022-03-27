import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1

Rectangle {
    id: mainRectangle

    property url avatarSource
    property string userName: "Unknown user"
    property string content: "Message text"
    property string dateTime: "00-00-0000 00:00"
    color: "#00ffffff"

    ColumnLayout
    {
        anchors.fill: parent

        RowLayout
        {
            Image {
                id: avatarImage
                source: mainRectangle.avatarSource

                width: 60
                height:60

                Layout.margins: 15
            }

            Label{
                id: userNameLabel
                text: mainRectangle.userName
            }

            Label{
                id: dateTimeLabel
                text: mainRectangle.dateTime
            }

            Rectangle{
                Layout.fillWidth: true
            }
        }


        TextArea{

            Layout.minimumWidth: 40

            id:contentTextArea
            text: mainRectangle.content
        }
    }
}
