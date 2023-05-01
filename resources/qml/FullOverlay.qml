import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: fullOverlayRectangle

    opacity: 1
    color: "#00000000"
    border.color: "#00000000"
    transformOrigin: Item.BottomRight
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    Layout.fillHeight: true

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Rectangle {
            id: fullOverlayHeader
            width: 260 * _uiBackend.sizeModifer
            height: 60 * _uiBackend.sizeModifer
            color: "#333333"
            Layout.fillWidth: true
            Layout.minimumWidth: 260 * _uiBackend.sizeModifer
            Layout.minimumHeight: 60 * _uiBackend.sizeModifer
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
            Layout.maximumHeight: 60 * _uiBackend.sizeModifer
            Layout.maximumWidth: 26099
            Layout.fillHeight: false


            RowLayout
            {
                anchors.fill: parent

                Rectangle {
                    id: buttonInfoRectangle

                    property bool hoverState : buttonInfoMouseArea.hovered
                    property bool pressedState : true

                    Layout.preferredWidth: 140 * _uiBackend.sizeModifer
                    color: pressedState ? "#ff080808" : "#00ffffff"
                    Layout.minimumWidth: 100 * _uiBackend.sizeModifer
                    Layout.minimumHeight: 60 * _uiBackend.sizeModifer

                    Text {
                        id: buttonInfoLabel
                        color: parent.hoverState ? "#ffffff" : "#999999"
                        opacity: 1
                        visible: true
                        text: qsTr("Information")
                        anchors.fill: parent
                        //font.pointSize: 14

                        font.pixelSize: 20 * _uiBackend.sizeModifer

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        layer.enabled: true
                        layer.effect: DropShadow {
                            verticalOffset: 2 * _uiBackend.sizeModifer
                            color: "#00000064"
                            radius: 1 * _uiBackend.sizeModifer
                            samples: 3 * _uiBackend.sizeModifer
                        }
                    }

                    GlobalMouseArea{
                        id: buttonInfoMouseArea
                        anchors.fill: parent

                        onClicked: {
                            buttonSettingsRectangle.pressedState = false;
                            buttonInfoRectangle.pressedState = !buttonInfoRectangle.pressedState;
                        }
                    }
                }

                Rectangle {
                    id: buttonSettingsRectangle

                    property bool hoverState : buttonSettingsMouseArea.hovered
                    property bool pressedState : false

                    Layout.preferredWidth: 140 * _uiBackend.sizeModifer
                    color: pressedState ? "#ff080808" : "#00ffffff"
                    Layout.minimumHeight: 60 * _uiBackend.sizeModifer
                    Layout.minimumWidth: 100 * _uiBackend.sizeModifer
                    Text {
                        id: buttonSettingsLabel
                        color: parent.hoverState ? "#ffffff" : "#999999"
                        opacity: 1
                        visible: true
                        text: qsTr("Settings")
                        anchors.fill: parent
                        //font.pointSize: 14
                        font.pixelSize: 20 * _uiBackend.sizeModifer
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        layer.enabled: true
                        layer.effect: DropShadow {
                            verticalOffset: 2 * _uiBackend.sizeModifer
                            color: "#00000064"
                            radius: 1 * _uiBackend.sizeModifer
                            samples: 3 * _uiBackend.sizeModifer
                        }
                    }

                    GlobalMouseArea{
                        id: buttonSettingsMouseArea
                        anchors.fill: parent

                        onClicked: {
                            buttonInfoRectangle.pressedState = false;
                            buttonSettingsRectangle.pressedState = !buttonSettingsRectangle.pressedState;
                        }
                    }
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }

                Label {
                    id: label
                    color: "#ebaf36"
                    text: qsTr("Press Alt+(~) key to roll up")
                    font.pixelSize: 12 * _uiBackend.sizeModifer
                    Layout.margins: 15 * _uiBackend.sizeModifer
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                }


            }
        }

        Rectangle {
            id: mainContentContainerRectangle
            width: 200 * _uiBackend.sizeModifer
            height: 200 * _uiBackend.sizeModifer
            opacity: 1
            color: "#00000000"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.rightMargin: 10 * _uiBackend.sizeModifer
            Layout.bottomMargin: 10 * _uiBackend.sizeModifer
            Layout.leftMargin: 10 * _uiBackend.sizeModifer
            Layout.topMargin: 0
            Layout.preferredHeight: 0
            Layout.preferredWidth: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            OverlaySettingsPage {
                id: overlaySettingsPage
                visible: buttonSettingsRectangle.pressedState
                anchors.fill: parent
            }

            InfoWindow {
                id: infoWindow
                visible: buttonInfoRectangle.pressedState
                anchors.fill: parent
                sizeModifer: _uiBackend.sizeModifer
                isOverlayPage: true
            }
        }
    }
}
