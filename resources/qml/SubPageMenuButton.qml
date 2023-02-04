import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: mainRectangle

    property bool pressedState : false
    property string text: "button"
    property bool newsAvailable: false

    signal clicked()

    width: 140
    color: pressedState ? "#000000" : "#272727"
    Layout.minimumWidth: 100
    Layout.minimumHeight: 60

    radius: 5

    Text {
        id: buttonInfoLabel
        color: "#ffffff"
        //opacity: 1
        text: mainRectangle.text
        anchors.fill: parent

        font.pixelSize: 16

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
