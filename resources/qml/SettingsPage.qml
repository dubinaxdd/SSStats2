import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: infoRectangle
    opacity: 1
    color: "#ffffff"
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10

    property var model: _uiBackend.settingsPageModel

    Connections
    {
        target: _uiBackend

        function onNoFogStateChanged()
        {
            noFogSwitch.checked = _uiBackend.noFogState;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onShowGamePannelPresetChanged()
        {
            showGamePannelSwitch.checked =  _uiBackend.gamePanel.showGamePannelPreset;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onSmallPannelActiveChanged()
        {
            smallPannelActiveSwitch.checked = _uiBackend.gamePanel.smallPannelActive;
        }
    }

    ColumnLayout
    {
        anchors.margins: 15
        anchors.fill: parent

        Switch{
            text: "Show overlay"
            checked: model.overlayVisible

            opacity: hovered? 1.0 : 0.8

            onCheckedChanged: {
                model.overlayVisible = checked;
            }
        }


        Switch{
            text: "Launch Soulstorm with Windows7 support mode"
            checked: model.win7SupportMode

            opacity: hovered? 1.0 : 0.8

            onCheckedChanged: {
                model.win7SupportMode = checked;
            }
        }

        Switch{
            text: "Skip the intro video"
            checked: model.skipIntroVideo

            opacity: hovered? 1.0 : 0.8

            onCheckedChanged: {
                model.skipIntroVideo = checked;
            }
        }

        Switch{
            id: noFogSwitch
            text: "No FOG"
            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged:
            {
                _uiBackend.noFogState = checked;
            }
        }

        Switch{
            id: showGamePannelSwitch
            text: "APM panel visible in game"
            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                _uiBackend.gamePanel.showGamePannelPreset = checked;
            }
        }

        Switch{
            id: smallPannelActiveSwitch
            text: "Small APM panel in game"
            //checked: _uiBackend.gamePanel.smallPannelActive
            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                _uiBackend.gamePanel.smallPannelActive = checked;
            }

        }

        Switch {
            id: enableEventsSoundsEhenGameMinimized
            text: "Enable events sounds when the game is minimized"
            checked: model.enableEventsSoundWhenGameMinimized

            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                model.enableEventsSoundWhenGameMinimized = checked;
            }
        }

        Switch {
            id: enableEventsSoundsEhenGameMaximized
            text: "Enable events sounds when the game is maximized"
            checked: model.enableEventsSoundWhenGameMaximized

            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                model.enableEventsSoundWhenGameMaximized = checked;
            }
        }

        Switch {
            enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked

            text: "Enable game load event sound"
            checked: model.enableGameLoadEventSound

            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                model.enableGameLoadEventSound = checked;
            }
        }

        Switch {

            enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked

            text: "Enable game start event sound"

            checked: model.enableGameStartEventSound

            opacity: hovered ? 1.0 : 0.8

            onCheckedChanged: {
                model.enableGameStartEventSound = checked;
            }
        }

        Rectangle
        {
            Layout.fillHeight: true
        }

    }
}
