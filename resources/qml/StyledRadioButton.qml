import QtQuick 2.0
import QtQuick.Controls 2.12
import DowStatsStyle 1.0

RadioButton{
    id: root
    property int fontPxSize: 11

    contentItem: Text {
        color: DowStatsStyle.textColor
        text: "  " + parent.text
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.indicator.right
        font.pixelSize: root.fontPxSize
    }
}
