import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

Switch{
    id: root
    property int fontPxSize: 11
    property real sizeModifer: 1
    property bool hoveredState: mouseArea.containsMouse
    property var textColor: DowStatsStyle.textColor
    property var backgroundCheckedColor: DowStatsStyle.greenColor
    property var backgroundUncheckedColor: DowStatsStyle.redColor
    property var backgroundDisblaedColor: DowStatsStyle.alternateBackgroundColor
    property var indicatorColor: DowStatsStyle.indicatorColor

    opacity: hoveredState ? 1.0 : 0.8

    contentItem: Text {
        color: root.textColor
        text: "  " + parent.text
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.background.right
        anchors.leftMargin: -30 * root.sizeModifer
        font.pixelSize: root.fontPxSize
    }

    onCheckedChanged: {
        if (checked)
            indicatorImage.state = "checked"
        else
            indicatorImage.state = ""
    }

    MouseArea{
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: root.checked = !root.checked
    }

    indicator: Rectangle{

        id: indicatorImage

        anchors.verticalCenter: parent.verticalCenter

        width: 100 * root.sizeModifer
        height: 100 * root.sizeModifer
        color: "#00000000"

        x: -35 * root.sizeModifer
        y: - 37 * root.sizeModifer

        Image
        {
            id: image
            anchors.fill: parent
            anchors.margins: 3 * root.sizeModifer
            source: "qrc:/componentScins/resources/componenScins/switch1.svg"
            sourceSize.width: 100 * root.sizeModifer
            sourceSize.height: 100 * root.sizeModifer
        }

        ColorOverlay{
            anchors.fill: image
            source:image
            color: root.enabled ? indicatorColor : backgroundDisblaedColor  //indicatorColor
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

        anchors.verticalCenter: parent.verticalCenter

        width: 100 * root.sizeModifer
        height: 100 * root.sizeModifer
        color: "#00000000"

        x: -22 * sizeModifer
        y: - 37 * sizeModifer

        Image
        {
            id: backgroundImage
            anchors.fill: parent
            anchors.margins: 3 * root.sizeModifer
            source: "qrc:/componentScins/resources/componenScins/switch2.svg"
            sourceSize.width: 100 * root.sizeModifer
            sourceSize.height: 100 * root.sizeModifer
        }

        ColorOverlay{
            anchors.fill: backgroundImage
            source:backgroundImage
            color: root.enabled ? (root.checked ? backgroundCheckedColor : backgroundUncheckedColor) : backgroundDisblaedColor
            antialiasing: true
        }
    }
}
