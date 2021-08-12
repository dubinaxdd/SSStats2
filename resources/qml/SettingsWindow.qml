import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: settingsRectangle
    opacity: 1
    color: "#f9f9f9"
    border.color: "#000000"
    border.width: 3
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    property int noFogSwitchX : settingsFlow.x + noFogSwitch.x
    property int noFogSwitchY : settingsFlow.y + noFogSwitch.y
    property int noFogSwitchWidth : noFogSwitch.width
    property int noFogSwitchHeight : noFogSwitch.height

    Connections{
        target: _uiBackend
    }


    Text {
        id: settingsLabel
        height: 40
        text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">НАСТРОЙКИ</span></p></body></html>"
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

    Flow {
        id: settingsFlow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: settingsLabel.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10

        Rectangle {
            id: tempRectangle
            width: 455
            height: 40
            opacity: 1
            color: "#f9f9f9"
            border.color: "#e17f7f"
            border.width: 1

            Switch {
                id: noFogSwitch
                text: qsTr("Убрать туман (размытие) игрового пространства при отдалении")
                font.pointSize: 10
                opacity: _uiBackend.switchNoFogHoverState ? 1.0 : 0.8
                checked: _uiBackend.switchNoFogState

            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
