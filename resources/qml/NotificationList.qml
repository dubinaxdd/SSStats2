import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0



ListView{
    model: _uiBackend.notificationManager
    verticalLayoutDirection: ListView.BottomToTop
    clip: true

    delegate: Rectangle {
        color: DowStatsStyle.selectionColor
        radius: 10
        width: 300
        height: 60

        ColumnLayout{
            anchors.fill: parent
            anchors.margins: 10

            Label{
                Layout.alignment: Qt.AlignVCenter
                text: model.text
                color: DowStatsStyle.textColor
                font.pixelSize: 14
            }
        }
    }
}
