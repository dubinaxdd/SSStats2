import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1
import DowStatsStyle 1.0
import GameType 1.0

Rectangle {
    id: root
    opacity: 1
    color: DowStatsStyle.backgroundColor
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10

    property var model: _uiBackend.gamePage
    property var settingsModel: _uiBackend.settingsPageModel

    Connections
    {
        target: _uiBackend

        function onNoFogStateChanged()
        {
            noFogSwitch.checked = _uiBackend.noFogState;
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10

        StyledComboBox{
            id: gameSelectComboBox

            Layout.fillWidth: true
            model: root.model
            textRole: "gameName"
            enabled: !_uiBackend.gameLaunchState
            onObjectSelected: root.model.currentItem = currentIndex;
            Component.onCompleted: currentIndex = root.model.currentItem
        }

        Item{
            Layout.preferredHeight: 30
        }

        Label{
            text: qsTr("Options:")
            color: DowStatsStyle.textColor
        }

        StyledSwitch{
            text: qsTr("Launch Game in window")
            checked: root.settingsModel.launchGameInWindow
            onCheckedChanged: root.settingsModel.launchGameInWindow = checked;
            enabled: root.model.currentGameType  !==  GameType.DefinitiveEdition
        }

        StyledSwitch{
            text: qsTr("Skip the intro video")
            checked: root.settingsModel.skipIntroVideo
            onCheckedChanged: root.settingsModel.skipIntroVideo = checked;
            enabled: root.model.currentGameType  !==  GameType.DefinitiveEdition
        }

        StyledSwitch{
            id: noFogSwitch
            text: qsTr("No FOG")
            onCheckedChanged: _uiBackend.noFogState = checked;
            //enabled: root.model.currentGameType  !==  GameType.DefinitiveEdition
        }

        StyledSwitch{
            id: advertisingSwitch
            text: qsTr("Auto ad sender")
            checked: root.settingsModel.enableAdvertising
            onCheckedChanged: root.settingsModel.enableAdvertising = checked;
            enabled: root.model.currentGameType  !==  GameType.DefinitiveEdition
        }

        ColumnLayout
        {
            clip: true
            visible: _uiBackend.gameIsInstalled && root.model.currentGameType !==  GameType.DefinitiveEdition
            Layout.topMargin: 10

            Label{
                Layout.preferredWidth: 250

                Layout.leftMargin: 5
                Layout.rightMargin: 5

                font.pixelSize: 11
                text: qsTr("Active mod: ") + _uiBackend.balanceModPage.currentModInGame
                color: DowStatsStyle.textColor
            }

            StyledComboBox{
                id: modComboBox
                model: [qsTr("Original Soulstorm"), qsTr("DoW Stats Balance Mod (Latest)"), qsTr("Current active mod")]

                Layout.preferredWidth: 250
                Layout.preferredHeight: 25

                onObjectSelected: _uiBackend.settingsPageModel.launchMode = currentIndex

                Connections{
                    target: _uiBackend.settingsPageModel
                    function onLaunchModeChanged(){
                        modComboBox.currentIndex = _uiBackend.settingsPageModel.launchMode
                    }
                }
            }
        }

        Item{
            Layout.fillHeight: true
        }

        RowLayout
        {
            IconButton{
                width: 250
                height: 35
                sourceUrl: "qrc:/icons/resources/images/game.svg"
                text: qsTr("Open Game folder")
                onClicked: _uiBackend.openGameFolder()
            }

            IconButton{
                width: 250
                height: 35
                sourceUrl: "qrc:/images/resources/images/gear.svg"
                text: qsTr("Open Settings folder")
                onClicked: _uiBackend.openSettingsFolder()
            }

            IconButton{
                width: 250
                height: 35
                sourceUrl: "qrc:/icons/resources/images/playback.svg"
                text: qsTr("Open Playback folder")
                onClicked: _uiBackend.openPlaybackFolder()
            }
        }
    }
}
