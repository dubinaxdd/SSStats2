import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle {
    id: infoRectangle

    property real sizeModifer: 1
    property bool isOverlayPage: false
    property var model: _uiBackend.onlineStatisticPanel

    width: 270
    height: 100

    opacity: 1
    color: DowStatsStyle.itemColor
    border.color: "#00000000"
    border.width: 0
    radius: 10

    ColumnLayout
    {
        id: layout

        anchors.fill: parent
        anchors.margins: 10

        CounterComponent{
            count: model.uniquePlayersOnDay
            text: qsTr("Unique players per day")
        }

        CounterComponent{
            count: model.uniquePlayersOnMonth
            text: qsTr("Unique players per month")
        }

        CounterComponent{
            count: model.uniquePlayersOnYear
            text: qsTr("Unique players per year")
        }

        CounterComponent{
            count: model.uniquePlayersOnTotal
            text: qsTr("Total number of unique players")
        }

        Label{
            id: label
            text: qsTr("(calculated starting from February 2026)")
            font.pixelSize: 9
            color: DowStatsStyle.textColor
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
