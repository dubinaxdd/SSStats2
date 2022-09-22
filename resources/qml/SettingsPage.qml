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

/*
        Switch{
            text: "No FOG"
            checked: _uiBackend.noFogState

            onCheckedChanged: {
                //_uiBackend.noFogState = checked;

                _uiBackend.setNoFogWithoutSignal(checked);
            }
        }

        Switch{
            text: "APM panel visible in game"
            checked: _uiBackend.gamePanel.showGamePannelPreset

            onCheckedChanged: {
                _uiBackend.gamePanel.showGamePannelPreset = checked;
            }
        }

        Switch{
            text: "Small APM panel in game"
            checked: _uiBackend.gamePanel.smallPannelActive

            onCheckedChanged: {
                _uiBackend.gamePanel.smallPannelActive = checked;
            }
        }
*/
        Rectangle
        {
            Layout.fillHeight: true
        }

    }

    //Режим совместимости с win7
    //Установить русские шрифты
    //Установить QWER хоткеи
    //Установить камерамод
    //Разблокировать расы

    //Включить оверлей в игре
    //Маленькая игровая панель
    //Отключтить туман
    //Включить игровую панель
    //Масштаб оверлея
}
