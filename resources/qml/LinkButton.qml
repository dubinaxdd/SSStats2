import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Rectangle{
    id: root
    Layout.preferredWidth: textLabel.width + 30 + (icon.visible ? rowLayout.spacing + icon.width : 0 )
    height: 35
    radius:5
    color: "#D2691E"

    property string link: ""
    property string text: ""
    property string iconSource: ""

    RowLayout
    {
        id: rowLayout
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5

        Image{
            id: icon
            visible: root.iconSource !== ""
            source: root.iconSource
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30
            Layout.leftMargin: 5
        }

        Label{
            id: textLabel
            text: root.text
            color: mouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
            font.pixelSize: 16
            Layout.leftMargin: icon.visible ? 0 : 15
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            Qt.openUrlExternally(root.link);
        }
    }
}
