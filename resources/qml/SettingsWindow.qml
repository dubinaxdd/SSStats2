import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: settingsRectangle
    opacity: 1
    color: "#f9f9f9"
    radius: 10
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    property Column settingsColumn : settingsColumn
    property Switch noFogSwitch : noFogSwitch
    property Switch testSwitch : testSwitch


    Connections{
        target: _uiBackend
    }


    Text {
        id: settingsLabel
        height: 40
        text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.RichText
        anchors.topMargin: 0
        anchors.leftMargin: 0
        anchors.rightMargin: 0
    }

    Column {
        id: settingsColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: settingsLabel.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10

        Switch {
            id: noFogSwitch

            property bool hoverState : false
            property bool checkedState : false

            text: qsTr("Remove fog (aka No Fog) of the game space when moving away")
            font.pointSize: 10
            opacity: hoverState ? 1.0 : 0.8
            checked: checkedState

        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
