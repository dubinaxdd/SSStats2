import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: fullOverlayRectangle
    width: 600
    height: 600
    opacity: 1
    color: "#00000000"
    border.color: "#00000000"
    transformOrigin: Item.BottomRight
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    Layout.fillHeight: true

    property Rectangle buttonInfo: buttonInfoRectangle
    property Rectangle buttonSettings : buttonSettingsRectangle

    property Rectangle mainContentContainerRectangle : mainContentContainerRectangle
    property SettingsWindow settingsWindow : settingsWindow

    Connections{
        target: _uiBackend
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Rectangle {
            id: fullOverlayHeader
            width: 260
            height: 60
            color: "#333333"
            Layout.fillWidth: true
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#333333"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            Layout.maximumHeight: 60
            Layout.maximumWidth: 26099
            Layout.fillHeight: false

            Rectangle {
                id: buttonInfoRectangle

                property bool hoverState : false
                property bool pressedState : false

                width: 140
                color: pressedState ? "#ff080808" : "#00ffffff"
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: 0
                anchors.bottomMargin: 0
                anchors.topMargin: 0

                Text {
                    id: buttonInfoLabel
                    color: parent.hoverState ? "#ffffff" : "#999999"
                    opacity: 1
                    visible: true
                    text: qsTr("Information")
                    anchors.fill: parent
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    layer.enabled: true
                    layer.effect: DropShadow {
                        verticalOffset: 2
                        color: "#00000064"
                        radius: 1
                        samples: 3
                    }
                }
            }

            Rectangle {
                id: buttonSettingsRectangle

                property bool hoverState : false
                property bool pressedState : false

                width: 140
                color: pressedState ? "#ff080808" : "#00ffffff"
                anchors.left: buttonInfoRectangle.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0

                Text {
                    id: buttonSettingsLabel
                    color: parent.hoverState ? "#ffffff" : "#999999"
                    opacity: 1
                    visible: true
                    text: qsTr("Settings")
                    anchors.fill: parent
                    font.pointSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    layer.enabled: true
                    layer.effect: DropShadow {
                        verticalOffset: 2
                        color: "#00000064"
                        radius: 1
                        samples: 3
                    }
                }
            }
        }

        Rectangle {
            id: mainContentContainerRectangle
            width: 200
            height: 200
            opacity: 1
            color: "#f9f9f9"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.rightMargin: 10
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.topMargin: 10
            Layout.preferredHeight: 0
            Layout.preferredWidth: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            SettingsWindow {
                id: settingsWindow
                visible: buttonSettingsRectangle.pressedState
                anchors.fill: parent
            }

            InfoWindow {
                id: infoWindow
                visible: buttonInfoRectangle.pressedState
                anchors.fill: parent
            }
        }
    }
}
