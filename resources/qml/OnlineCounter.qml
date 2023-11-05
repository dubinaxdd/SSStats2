import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: mainRectangle

    property bool pressedState : false
    property bool enabled: true

    signal clicked()

    Layout.preferredWidth: imageRectangle.width + counterLabel.width + 20
    Layout.minimumHeight: 60


    color: {
        if(!enabled)
            return "#3A3B3C"

        if (pressedState)
            return "#ff080808"
        else
            return "#00ffffff"
    }

    RowLayout
    {
        id: layout
        anchors.fill: parent
        anchors.margins: 5

        Rectangle
        {
            id: imageRectangle

            width: 25
            height: 25
            color:"#00000000"

            Image {
               anchors.fill: parent
               source: "qrc:/images/resources/images/man_user.svg"
               sourceSize.width: 25
               sourceSize.height: 25
            }
        }

        Label
        {
            id: counterLabel
            text: _uiBackend.onlineCount
            font.pixelSize: 15
            color: {
                if (mouseArea.containsMouse  || pressedState)
                     return "#ffffff"
                else
                    return "#999999"
            }
            Layout.rightMargin: 15
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        ToolTip.visible: containsMouse
        ToolTip.delay: 1000
        ToolTip.text: ("Online DoW Stats users")

        cursorShape: Qt.PointingHandCursor

        onClicked: {
            if (enabled)
                pressedState = !pressedState;
            else
                pressedState = false;

            mainRectangle.clicked();
        }
    }
}

