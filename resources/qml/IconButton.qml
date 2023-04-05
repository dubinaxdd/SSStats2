import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.15

Rectangle
{
    id: buttonRectangle

    property string sourceUrl: "";
    property string toolTipText: "";

    signal clicked()

    width: 40
    height: 40
    radius: 10

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
        anchors.margins: 5
        source: sourceUrl
        sourceSize.width: 30
        sourceSize.height: 30


    }

    ColorOverlay{
        anchors.fill: image
        source:image
        color: buttonMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
        //transform:rotation
        antialiasing: true
    }

    MouseArea
    {
        id: buttonMouseArea

        anchors.fill: parent
        hoverEnabled: true

        ToolTip.visible: containsMouse
        ToolTip.delay: 1000
        ToolTip.text: toolTipText

        onClicked:
        {
            buttonRectangle.clicked();
        }
    }
}

