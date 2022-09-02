import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: infoRectangle
    opacity: 1
    color: "#ffffff"
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10


    RowLayout
    {
        anchors.fill: parent

        ColumnLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.margins: 15

            RowLayout
            {

                spacing: 30

                ColumnLayout
                {
                    Rectangle
                    {
                        Layout.preferredWidth: 140
                        Layout.preferredHeight: 140

                        color: "#112332"
                        radius: 10

                        Label{
                             anchors.fill: parent
                             text: "Map"
                         }
                    }

                    Label{
                        text: "Map_name"
                        Layout.alignment: Qt.AlignCenter
                    }

                }

                ColumnLayout
                {
                    Layout.fillWidth: true

                    Label{
                        text: "Name"
                        font.pixelSize: 30
                    }

                    Label{
                        text: "mod"
                    }

                    Label{
                        text: "version"
                    }

                    Label{
                        text: "duration"
                    }

                    Label{
                        text: "settings"
                    }

                    Label{
                        text: "win conditions"
                    }
                }
            }

            ListView {
                width: 180; height: 200

                model: asd


                delegate: Text {
                    text: name + ": " + number
                }
            }
        }

        ColumnLayout
        {
            Layout.fillHeight: true
            Layout.preferredWidth: 200

            ListView
            {
                Layout.fillHeight: true
                model: ["реплей 1", "реплей 2", "реплей 3"]
            }
        }
    }
}
