import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle{
    property string text: ""
    signal clicked()

    id: rootObject
    Layout.preferredHeight: 25
    Layout.preferredWidth: 70
    radius:3

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
        Label{
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            text: rootObject.text
            color: buttonMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
        }
    }

    MouseArea{
        id: buttonMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            rootObject.clicked();
        }
    }
}
