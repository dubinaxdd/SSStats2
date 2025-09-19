import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: mainRectangle

    width: 280
    Layout.minimumWidth: 280
    Layout.maximumWidth: 280

    visible: model.curentPlayerStatsItem.itemVisible && model.curentPlayerStatsItem.playerName !== ""

    color: "#00000000"
    border.color: "#00000000"

    property var model

    ScrollView {
        id: scrollView

        height: mainRectangle.height
        width: mainRectangle.width

        clip: true

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent
            spacing: 5

            PlayersStatisticColumn
            {
                hoverEnabled: true
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}


