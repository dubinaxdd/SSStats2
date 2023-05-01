import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle {

    id: root
    property var model: _uiBackend.onlineStatisticPanel
    color: DowStatsStyle.backgroundColor
    radius: 10

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10

        Label{
            text: "Online statistics"
            font.pixelSize: 16
            color: DowStatsStyle.textColor
        }

        ListView
        {
            id: listView
            model: root.model
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 1
            clip: true

            delegate: Rectangle{
                width: listView.width - (scrollBar.visible ?  scrollBar.width + 5 : 0)
                height: visible ? 20 : 0
                color: "#00000000"
                clip: true

                visible: model.modName !== "DoW Stats Balance Mod"

                RowLayout{
                    anchors.fill: parent
                    spacing: 5

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
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Rectangle{
                        Layout.fillWidth: true
                    }

                    Label{
                        text: model.onlineCount //+ (model.modName === "DoW Stats Balance Mod" ? 15 : 0)
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
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
