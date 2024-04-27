import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.15

Rectangle
{
    id: buttonRectangle

    property string sourceUrl: "";
    property string toolTipText: "";

    property bool containsMouse: false;
    property real sizeModifer: 1.0;

    signal clicked()

    width: 40
    height: 40
    radius: 10 * buttonRectangle.sizeModifer

    //color: buttonMouseArea.containsMouse ? "#A9A9A9" : "#E0E0E0"

    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#337ab7"
        }

        GradientStop {
            position: 1
            color: "#245580"
        }
    }

    Image {

        id: image
        anchors.fill: parent
        anchors.margins: 5 * buttonRectangle.sizeModifer
        source: sourceUrl
        sourceSize.width: width//* buttonRectangle.sizeModifer
        sourceSize.height: height //* buttonRectangle.sizeModifer
    }

    ColorOverlay{
        anchors.fill: image
        source:image
        color: buttonMouseArea.containsMouse || buttonRectangle.containsMouse ? "#DCDCDC" : "#FFFFFF"
        //transform:rotation
        antialiasing: true
    }

    MouseArea
    {
        id: buttonMouseArea

        anchors.fill: parent
        hoverEnabled: true

        ToolTip.visible: containsMouse
        ToolTip.delay: 600
        ToolTip.text: toolTipText

        cursorShape: Qt.PointingHandCursor

        onClicked:
        {
            buttonRectangle.clicked();
        }
    }
}

