import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

ComboBox{
    id: root

    indicator: Rectangle{

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 10

        width: 20
        height: 20
        color: "#00000000"

        Image
        {
            id: image
            anchors.fill: parent
            anchors.margins: 3
            source: "qrc:/images/resources/images/arrow_down.svg"
            sourceSize.width: 14
            sourceSize.height: 14
        }

        ColorOverlay{
            anchors.fill: image
            source:image
            color: DowStatsStyle.textColor
            //transform:rotation
            antialiasing: true
        }

    }

    contentItem: Text {
        color: DowStatsStyle.textColor
        text: "  " + root.currentText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 12
    }

    background: Rectangle {
        radius: 10
        implicitHeight: 40
        border.color: "#00000000"
        border.width: 0
        color: DowStatsStyle.alternateBackgroundColor
    }
}
