import QtQuick 2.0
import QtQuick.Controls 2.12
import DowStatsStyle 1.0
import QtGraphicalEffects 1.15

RadioButton{
    id: root
    property int fontPxSize: 11
    property real sizeModifer: 1

    property var backgroundColor: DowStatsStyle.itemColor
    property var indicatorColor: DowStatsStyle.indicatorColor

    contentItem: Text {
        color: DowStatsStyle.textColor
        text: "  " + parent.text
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.indicator.right
        font.pixelSize: root.fontPxSize
    }

    indicator: Rectangle{

        id: indicatorImage

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: backgroundRectangle.horizontalCenter

        width: 30 * root.sizeModifer
        height: 30 * root.sizeModifer
        color: "#00000000"

        Image
        {
            id: image
            anchors.fill: parent

            anchors.margins: 3 * root.sizeModifer
            source: root.checked ? "qrc:/componentScins/resources/componenScins/radioButton2.svg" : "qrc:/componentScins/resources/componenScins/radioButton1.svg"
            sourceSize.width: 30 * root.sizeModifer
            sourceSize.height: 30 * root.sizeModifer
        }

        ColorOverlay{
            anchors.fill: image
            source:image
            color: root.indicatorColor
            antialiasing: true
        }

        states: [

            State { name: "checked"
                PropertyChanges {
                    target: indicatorImage
                    x: -10 * root.sizeModifer
                }
            }
        ]
        transitions: [

            Transition {
                NumberAnimation { properties: "x" }
            }
        ]
    }

    background: Rectangle{
        id: backgroundRectangle

        anchors.verticalCenter: parent.verticalCenter

        width: 30 * root.sizeModifer
        height: 30 * root.sizeModifer
        color: "#00000000"

        Image
        {
            id: backgroundImage
            anchors.fill: parent
            anchors.margins: 3 * root.sizeModifer
            source: "qrc:/componentScins/resources/componenScins/radioButton3.svg"
            sourceSize.width: 30 * root.sizeModifer
            sourceSize.height: 30 * root.sizeModifer
        }

        ColorOverlay{
            anchors.fill: backgroundImage
            source:backgroundImage
            color: DowStatsStyle.backgroundColor
            antialiasing: true
        }
    }
}
