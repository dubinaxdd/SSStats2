import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: mainRectangle

    property bool pressedState : false
    property string text: "button"

    signal clicked()

    width: 140
    color: pressedState ? "#ff080808" : "#00ffffff"
    Layout.minimumWidth: 100
    Layout.minimumHeight: 60

    Text {
        id: buttonInfoLabel
        color: mouseArea.containsMouse ? "#ffffff" : "#999999"
        //opacity: 1
        text: mainRectangle.text
        anchors.fill: parent

        font.pixelSize: 20

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
