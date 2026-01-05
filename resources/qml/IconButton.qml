import QtQuick 2.0
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15

Rectangle
{
    id: root

    property string sourceUrl: "";
    property string toolTipText: "";
    property string text;

    property bool containsMouse: false;
    property real sizeModifer: 1.0;

    signal clicked()

    width: 40
    height: 40
    radius: 5 * root.sizeModifer

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

    RowLayout
    {
        anchors.fill: parent
        spacing: 15


        Rectangle
        {
            width: root.height * 0.7
            height: root.height * 0.7
            color: "#00000000"

            Layout.leftMargin: root.height * 0.15

            Image {
                id: image
                anchors.fill: parent
                source: root.sourceUrl
                sourceSize.width: parent.height
                sourceSize.height: parent.height
            }

            ColorOverlay{
                anchors.fill: image
                source:image
                color: buttonMouseArea.containsMouse || buttonMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                antialiasing: true
            }
        }

        Label{
            id: textLabel
            text: root.text
            Layout.rightMargin: 15

            visible: root.text != ""

            color: buttonMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
            font.pixelSize: 16
        }
    }

    MouseArea{
        id: buttonMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: root.clicked()
    }
}

