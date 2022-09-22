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

    Connections {
        target: model

        function onUnlockRacesStatusChanged()
        {
            unlockRacesButton.enabled = true;
        }
    }

    ColumnLayout
    {
        anchors.margins: 15
        anchors.fill: parent

        spacing: 30

        Rectangle
        {
            Layout.preferredWidth: unlockerLabel.width + unlockRacesButton.width + unlockerStateLabel.width + 50
            Layout.preferredHeight: 60

            radius: 10
            color: "#00ff99"

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                Label{
                    id: unlockerLabel
                    text: "Race unlocker:"
                    font.pointSize: 15
                }

                Button{
                    id: unlockRacesButton
                    text: "Unlock races"
                    height: 15

                    opacity: hovered? 1.0 : 0.9
                    onClicked: {
                        model.unlockRaces();
                        enabled = false;
                    }
                }

                Label{
                    id: unlockerStateLabel
                    text: model.unlockRacesStatus
                }

            }
        }

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
                height: 15
                opacity: hovered? 1.0 : 0.8

                onClicked: {
                    model.installRussianFonts();
                }
            }

            Button{
                text: "Delete"
                height: 15
                enabled: model.russianFontsInstalledStatus
                opacity: hovered? 1.0 : 0.8

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
                height: 15
                opacity: hovered? 1.0 : 0.8

                onClicked: {
                    model.installCameraMod();
                }
            }

            Button{
                text: "Delete"
                height: 15
                enabled: model.cameraModInstalledStatus
                opacity: hovered? 1.0 : 0.8

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
                height: 15
                opacity: hovered? 1.0 : 0.8

                onClicked: {
                    model.installGridHotkeys();
                }
            }

            Button{
                text: "Delete"
                height: 15
                enabled: model.gridHotkeysInstalledStatus
                opacity: hovered? 1.0 : 0.8
                onClicked: {
                    model.uninstallGridHotkeys();
                }
            }
        }

        Rectangle{
            Layout.fillHeight: true
        }
    }
}
