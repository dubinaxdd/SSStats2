import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: mainRectangle

    property bool pressedState : false
    property string text: "button"
    property bool newsAvailable: false

    signal clicked()

    width: buttonInfoLabel.width + 40//140
    color: pressedState ? "#ff080808" : "#00ffffff"
    Layout.minimumHeight: 60

    ColumnLayout
    {
        anchors.fill: parent

        Text {
            id: buttonInfoLabel
            color: mouseArea.containsMouse || pressedState  ? "#ffffff" : "#999999"
            text: mainRectangle.text
            Layout.alignment: Qt.AlignCenter

            font.pixelSize: 17

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            layer.enabled: true
            layer.effect: DropShadow {
                verticalOffset: 2
                color: "#00000064"
                radius: 1
                samples: 3
            }
        }
    }

    Rectangle{

        width: 10
        height: 10

        anchors.top: parent.top
        anchors.right: parent.right

        anchors.margins: 5

        visible: newsAvailable

        color: "#00000000"

        Image {
            id: name
            anchors.fill: parent
            source: "qrc:/images/resources/images/redDot.png"
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        cursorShape: Qt.PointingHandCursor

        onClicked: {
            pressedState = true;
            mainRectangle.clicked();
        }
    }

}
