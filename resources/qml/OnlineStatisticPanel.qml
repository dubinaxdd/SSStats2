import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
Rectangle {

    id: root
    property var model: _uiBackend.onlineStatisticPanel
    color: "#ffffff"
    radius: 10

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10

        Label{
            text: "Online statistics"
            font.pixelSize: 16
        }

        ListView
        {
            id: listView
            model: root.model
            Layout.fillWidth: true
            Layout.fillHeight: true

            delegate: Rectangle{
                width: listView.width - (scrollBar.visible ?  scrollBar.width + 5 : 0)
                height: 20
                color: "#00000000"
                clip: true

                RowLayout{
                    anchors.fill: parent

                    Rectangle{
                        radius:5

                        Layout.preferredHeight: 10
                        Layout.preferredWidth: 10

                        color: "#00ff99"
                    }

                    Label{
                        text: model.modName
                        Layout.preferredWidth: 200
                        clip: true
                    }

                    Rectangle{
                        Layout.fillWidth: true
                    }

                    Label{
                        text: model.onlineCount
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
                visible: listView.contentItem.height > listView.height
                policy: ScrollBar.AlwaysOn
            }
        }
    }
}
