import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

ListView{
    id: root
    model: _uiBackend.notificationManager
    verticalLayoutDirection: ListView.BottomToTop
    clip: true

    width: 300

    height: contentHeight > maxHeight ? maxHeight : contentHeight

    property int maxHeight

    delegate: Rectangle {

        id: delegate
        color: DowStatsStyle.selectionColor
        radius: 10
        width: root.width
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

        MouseArea{
            anchors.fill: parent

            onClicked: {
                root.model.removeItem(model.index);
            }
        }
    }

    MouseArea{
        anchors.fill: parent
        propagateComposedEvents: true
    }
}
