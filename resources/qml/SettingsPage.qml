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

    property var model: _uiBackend.settingsPageModel

    ColumnLayout
    {
        anchors.margins: 15
        anchors.fill: parent

        GridLayout{

            columns: 4

            //Шрифты
            Label{
                text: "Russian fonts:"
            }

            Label{
                id: progressLabel0;
                text: model.russianFontsInstallProgress
            }

            Button{
                text: model.russianFontsInstalledStatus ? "Reinstall" : "Install"
                enabled: !model.russianFontsInstallInProcess
                height: 20

                onClicked: {
                    model.installRussianFonts();
                }
            }

            Button{
                text: "Delete"
                height: 20
                enabled: model.russianFontsInstalledStatus

                onClicked: {
                    model.uninstallRussianFonts();
                }
            }

            //Камера мод
            Label{
                text: "Advanced camera mod:"
            }

            Label{
                id: progressLabel1;
                text: model.cameraModInstallProgress
            }

            Button{
                text: model.cameraModInstalledStatus ? "Reinstall" : "Install"
                enabled: !model.cameraModInstallInProcess
                height: 20

                onClicked: {
                    model.installCameraMod();
                }
            }

            Button{
                text: "Delete"
                height: 20
                enabled: model.cameraModInstalledStatus

                onClicked: {
                    model.uninstallCameraMod();
                }
            }


            //Хоткеи
            Label{
                text: "Grid hotkeys:"
            }

            Label{
                id: progressLabel2;
                text: model.gridHotkeysInstallProgress
            }

            Button{
                text: model.gridHotkeysInstalledStatus ? "Reinstall" : "Install"
                enabled: !model.gridHotkeysInstallInProcess
                height: 20

                onClicked: {
                    model.installGridHotkeys();
                }
            }

            Button{
                text: "Delete"
                height: 20
                enabled: model.gridHotkeysInstalledStatus

                onClicked: {
                    model.uninstallGridHotkeys();
                }
            }

    /*
            Switch{
                text: "Install QWER hotkeys "
            }

            Switch{
                text: "Unlock races"
            }*/
        }

        Switch{
            text: "Show overlay"
            checked: model.overlayVisible

            onCheckedChanged: {
                model.overlayVisible = checked;
            }
        }


        Switch{
            text: "Launch Soulstorm with Windows7 support mode"
            checked: model.win7SupportMode

            onCheckedChanged: {
                model.win7SupportMode = checked;
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


    radius: 10
}
