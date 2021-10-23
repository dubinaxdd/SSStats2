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

    property int relativeMouseX
    property int relativeMouseY

    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        if (buttonSettingsRectangle.pressedState)
            settingsWindow.mouseClick(relativeMouseX - settingsWindow.x - mainContentContainerRectangle.x, relativeMouseY - settingsWindow.y - mainContentContainerRectangle.y);

        // Кнопка "Настройки" - отобразить окно с настройками
        if (relativeMouseX >= buttonSettingsRectangle.x &&
                relativeMouseX <= buttonSettingsRectangle.x + buttonSettingsRectangle.width &&
                relativeMouseY >= buttonSettingsRectangle.y &&
                relativeMouseY <= buttonSettingsRectangle.y + buttonSettingsRectangle.height)
        {
            buttonInfoRectangle.pressedState = false;
            buttonSettingsRectangle.pressedState = !buttonSettingsRectangle.pressedState;
        }

        // Кнопка "Информация" - отобразить окно с информацией
        if (relativeMouseX >= buttonInfoRectangle.x &&
                relativeMouseX <= buttonInfoRectangle.x + buttonInfoRectangle.width &&
                relativeMouseY >= buttonInfoRectangle.y &&
                relativeMouseY <= buttonInfoRectangle.y + buttonInfoRectangle.height)
        {
            buttonSettingsRectangle.pressedState = false;
            buttonInfoRectangle.pressedState = !buttonInfoRectangle.pressedState;
        }

    }

    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        if (buttonSettingsRectangle.pressedState)
            settingsWindow.mouseHover(relativeMouseX - settingsWindow.x - mainContentContainerRectangle.x, relativeMouseY - settingsWindow.y - mainContentContainerRectangle.y);

        // Кнопка "Настройки"
        if (relativeMouseX >= buttonSettingsRectangle.x &&
                relativeMouseX <= buttonSettingsRectangle.x + buttonSettingsRectangle.width &&
                relativeMouseY >= buttonSettingsRectangle.y &&
                relativeMouseY <= buttonSettingsRectangle.y + buttonSettingsRectangle.height)
        {
            buttonSettingsRectangle.hoverState = true;
        }else{
            buttonSettingsRectangle.hoverState = false;
        }

        // Кнопка "Информация"
        if (relativeMouseX >= buttonInfoRectangle.x &&
                relativeMouseX <= buttonInfoRectangle.x + buttonInfoRectangle.width &&
                relativeMouseY >= buttonInfoRectangle.y &&
                relativeMouseY <= buttonInfoRectangle.y + buttonInfoRectangle.height)
        {
            buttonInfoRectangle.hoverState = true;
        } else {
            buttonInfoRectangle.hoverState = false;
        }

    }




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

            RowLayout
            {
                anchors.fill: parent

                Rectangle {
                    id: buttonInfoRectangle

                    property bool hoverState : false
                    property bool pressedState : true

                    width: 140
                    color: pressedState ? "#ff080808" : "#00ffffff"
                    Layout.minimumWidth: 100
                    Layout.minimumHeight: 60

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
                    Layout.minimumHeight: 60
                    Layout.minimumWidth: 100
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

                Rectangle
                {
                    Layout.fillWidth: true

                }

                Label {
                    id: label
                    color: "#ebaf36"
                    text: qsTr("Press Alt+(~) key to roll up")
                    Layout.margins: 15
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                }


            }
        }

        Rectangle {
            id: mainContentContainerRectangle
            width: 200
            height: 200
            opacity: 1
            color: "#00000000"
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.rightMargin: 10
            Layout.bottomMargin: 10
            Layout.leftMargin: 10
            Layout.topMargin: 0
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
