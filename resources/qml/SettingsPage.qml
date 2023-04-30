import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle {
    id: root
    opacity: 1
    color: DowStatsStyle.backgroundColor//"#ffffff"
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

    Flickable
    {
        clip: true

        ScrollBar.vertical: ScrollBar{
            visible: settingsContent.height > height
            policy: ScrollBar.AlwaysOn
        }

        anchors.margins: 15
        anchors.fill: parent

        contentHeight: settingsContent.height

        ColumnLayout
        {
            id: settingsContent

            StyledComboBox{
                id: themeComboBox
                Layout.preferredWidth: 255

                model: ["Theme: Light", "Theme: Dark", "Theme: Pink"]

                currentIndex: root.model.currentTheme

                onCurrentIndexChanged: {
                    switch (currentIndex)
                    {
                        case(0): DowStatsStyle.setLightTheme(); break;
                        case(1): DowStatsStyle.setDarkTheme();  break;
                        case(2): DowStatsStyle.setPinkTheme();  break;
                    }

                    root.model.currentTheme = themeComboBox.currentIndex;
                }
            }

            StyledSwitch{
                text: "Show overlay"
                checked: model.overlayVisible
                onCheckedChanged: model.overlayVisible = checked;
            }

            StyledSwitch{
                text: "Launch Soulstorm with Windows7 support mode"
                checked: model.win7SupportMode
                onCheckedChanged: model.win7SupportMode = checked;
            }

            StyledSwitch{
                text: "Launch Soulstorm in window"
                checked: model.launchGameInWindow
                onCheckedChanged: model.launchGameInWindow = checked;
            }

            StyledSwitch{
                text: "Skip the intro video"
                checked: model.skipIntroVideo
                onCheckedChanged: model.skipIntroVideo = checked;
            }

            StyledSwitch{
                id: noFogSwitch
                text: "No FOG"
                onCheckedChanged: _uiBackend.noFogState = checked;
            }

            StyledSwitch{
                id: showGamePannelSwitch
                text: "APM panel visible in game"
                onCheckedChanged: _uiBackend.gamePanel.showGamePannelPreset = checked;
            }

            StyledSwitch{
                id: smallPannelActiveSwitch
                text: "Small APM panel in game"
                onCheckedChanged: _uiBackend.gamePanel.smallPannelActive = checked;
            }

            StyledSwitch {
                id: enableEventsSoundsEhenGameMinimized
                text: "Enable events sounds when the game is minimized"
                checked: model.enableEventsSoundWhenGameMinimized
                onCheckedChanged: model.enableEventsSoundWhenGameMinimized = checked;
            }

            StyledSwitch {
                id: enableEventsSoundsEhenGameMaximized
                text: "Enable events sounds when the game is maximized"
                checked: model.enableEventsSoundWhenGameMaximized
                onCheckedChanged: model.enableEventsSoundWhenGameMaximized = checked;
            }

            StyledSwitch {
                enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked
                text: "Enable game load event sound"
                checked: model.enableGameLoadEventSound
                onCheckedChanged: model.enableGameLoadEventSound = checked;
            }

            StyledSwitch {
                enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked
                text: "Enable game start event sound"
                checked: model.enableGameStartEventSound
                onCheckedChanged: model.enableGameStartEventSound = checked;
            }

            RowLayout
            {
                Label
                {
                    text: "Events volume:"
                    opacity: 0.8
                    font.pixelSize: 12
                    color: DowStatsStyle.textColor
                }

                Slider {
                    from: 0
                    value: model.volume
                    to: 100
                    opacity: hovered ? 1.0 : 0.8
                    onValueChanged: model.volume = value;
                }
            }

            Rectangle
            {
                Layout.fillHeight: true
            }
        }
    }
}
