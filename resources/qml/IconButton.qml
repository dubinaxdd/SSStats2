import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle
{
    id: buttonRectangle

    property string sourceUrl: "";
    property string toolTipText: "";

    signal clicked()

    width: 40
    height: 40
    color: buttonMouseArea.containsMouse ? "#A9A9A9" : "#E0E0E0"
    radius: 5

    Image {
       anchors.fill: parent

       anchors.margins: 5

       source: sourceUrl
       sourceSize.width: 30
       sourceSize.height: 30
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

