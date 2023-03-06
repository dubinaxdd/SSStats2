import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    property var model: _uiBackend.settingsPageModel

    Connections {
        target: model

        function onUnlockRacesStatusChanged()
        {
            unlockRacesButton.enabled = true;
        }
    }

    ColumnLayout{
        id: columnLayout
        anchors.fill: parent

        Rectangle
        {
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            radius: 10
            color: "#00ff99"

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                Label{
                    id: unlockerLabel
                    text: "Race unlocker:"
                    font.pointSize: 12
                    Layout.preferredWidth: 200
                }

                Label{
                    id: unlockerStateLabel
                    text: model.unlockRacesStatus
                    Layout.preferredWidth: 200
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }

                BlueButton{
                    id: unlockRacesButton
                    text: "Unlock races"
                    Layout.preferredWidth: 140 + columnLayout.spacing

                    onClicked: {
                        model.unlockRaces();
                        enabled = false;
                    }
                }
            }
        }

        Rectangle
        {
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            color: "#DCDCDC"
            radius: 10

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                //Шрифты
                Label{
                    text: "Russian fonts:"
                    Layout.preferredWidth: 200
                }

                Label{
                    id: progressLabel0;
                    text: model.russianFontsInstallProgress
                    Layout.preferredWidth: 200
                }

                Rectangle{
                    Layout.fillWidth: true
                }

                BlueButton{
                    text: model.russianFontsInstalledStatus ? "Reinstall" : "Install"
                    enabled: !model.russianFontsInstallInProcess

                    onClicked: {
                        model.installRussianFonts();
                    }
                }

                BlueButton{
                    text: "Delete"
                    enabled: model.russianFontsInstalledStatus

                    onClicked: {
                        model.uninstallRussianFonts();
                    }
                }
            }
        }

        Rectangle
        {
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            color: "#DCDCDC"
            radius: 10

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                //Камера мод
                Label{
                    text: "Advanced camera mod:"
                    Layout.preferredWidth: 200
                }

                Label{
                    id: progressLabel1;
                    text: model.cameraModInstallProgress
                    Layout.preferredWidth: 200
                }

                Rectangle{
                    Layout.fillWidth: true
                }

                BlueButton{
                    text: model.cameraModInstalledStatus ? "Reinstall" : "Install"
                    enabled: !model.cameraModInstallInProcess

                    onClicked: {
                        model.installCameraMod();
                    }
                }

                BlueButton{
                    text: "Delete"
                    enabled: model.cameraModInstalledStatus

                    onClicked: {
                        model.uninstallCameraMod();
                    }
                }
            }
        }

        Rectangle
        {
            Layout.preferredHeight: 50
            Layout.fillWidth: true
            color: "#DCDCDC"
            radius: 10

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                //Хоткеи
                Label{
                    text: "Grid hotkeys:"
                    Layout.preferredWidth: 200
                }

                Label{
                    id: progressLabel2;
                    text: model.gridHotkeysInstallProgress
                    Layout.preferredWidth: 200
                }

                Rectangle{
                    Layout.fillWidth: true
                }

                BlueButton{
                    text: model.gridHotkeysInstalledStatus ? "Reinstall" : "Install"
                    enabled: !model.gridHotkeysInstallInProcess

                    onClicked: {
                        model.installGridHotkeys();
                    }
                }

                BlueButton{
                    text: "Delete"
                    enabled: model.gridHotkeysInstalledStatus

                    onClicked: {
                        model.uninstallGridHotkeys();
                    }
                }
            }
        }

        Rectangle{
            Layout.fillHeight: true
        }
    }
}
