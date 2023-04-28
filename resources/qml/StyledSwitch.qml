import QtQuick 2.0
import QtQuick.Controls 2.12
import DowStatsStyle 1.0

Switch{
    contentItem: Text {
        color: DowStatsStyle.textColor
        text: "  " + parent.text
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.indicator.right
        font.pixelSize: 11
    }
}
