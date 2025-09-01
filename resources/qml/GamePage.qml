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

            onCurrentIndexChanged: {
                model.updateCurrentGame(currentIndex);
            }
        }

        ColumnLayout
        {
            clip: true

            visible: _uiBackend.soulstormIsInstalled

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

        StyledSwitch{
            text: qsTr("Launch Soulstorm in window")
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
            enabled: root.model.currentGameType  !==  GameType.DefinitiveEdition
        }

        Item{
            Layout.fillHeight: true
        }

        Rectangle{
            id: lounchGameButtonRectangle
            Layout.preferredWidth:  15 + lounchGameLabel.width + 15 + iconImage.width + 5
            Layout.preferredHeight: 35
            Layout.rightMargin: 15
            radius:5

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
                spacing: 15

                Label{
                    id: lounchGameLabel
                    text: qsTr("Play")
                    Layout.leftMargin: 15
                    color: lounchGameMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                    font.pixelSize: 16
                }

                Rectangle
                {
                    width: 30
                    height: 30
                    color: "#00000000"

                    Layout.rightMargin: 15

                    Image{
                        id: iconImage
                        anchors.fill: parent
                        source: "qrc:/images/resources/icons/ssIcon.png"
                    }
                }

                MouseArea{
                    id: lounchGameMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onClicked: {
                        _uiBackend.launchGame()
                    }
                }
            }
        }
    }
}
