import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle{

    property string text: ""
    signal clicked()

    id: rootObject
    Layout.preferredHeight: 25
    Layout.preferredWidth: textLabel.width + 30
    //width: textLabel.width + 30
    radius:3

    gradient: Gradient {
        GradientStop {
            position: 0
            color: rootObject.enabled ? "#337ab7" : "#D3D3D3"
        }

        GradientStop {
            position: 1
            color:  rootObject.enabled ? "#245580" : "#A9A9A9"
        }
    }

    RowLayout
    {
        anchors.fill: parent
        Label{
            id: textLabel
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            text: rootObject.text
            color: buttonMouseArea.containsMouse && rootObject.enabled ? "#DCDCDC" : "#FFFFFF"
            font.pixelSize: 11
        }
    }

    MouseArea{
        id: buttonMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            if (rootObject.enabled)
                rootObject.clicked();
        }
    }
}
