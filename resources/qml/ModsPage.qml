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


    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 40

        Rectangle
        {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            //Layout.topMargin: 15
            color: "#eaeaea"
            radius: 10
            Layout.preferredWidth: 220
            Layout.fillHeight: true

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                SubPageMenuButton
                {
                    id: balanceModButton

                    text: "DowStats balance mod"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    pressedState: true

                    onClicked: {
                        mapsButton.pressedState = false;
                        otherModsButton.pressedState = false;

                        dowStatsModSubPage.visible = true;
                        mapsSubPage.visible = false;
                        otherModsSubPage.visible = false;
                    }
                }

                SubPageMenuButton
                {
                    id: mapsButton
                    text: "Maps"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    onClicked: {
                        balanceModButton.pressedState = false;
                        otherModsButton.pressedState = false;

                        dowStatsModSubPage.visible = false;
                        mapsSubPage.visible = true;
                        otherModsSubPage.visible = false;
                    }
                }

                SubPageMenuButton
                {
                    id: otherModsButton
                    text: "Other mods"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    onClicked: {
                        balanceModButton.pressedState = false;
                        mapsButton.pressedState = false;

                        dowStatsModSubPage.visible = false;
                        mapsSubPage.visible = false;
                        otherModsSubPage.visible = true;
                    }
                }

                Rectangle{
                    Layout.fillHeight: true
                }
            }
        }

        ColumnLayout
        {
            DowStatsModSubPage
            {
                id: dowStatsModSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: true
            }

            MapsSubPage
            {
                id: mapsSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: false
            }

            OtherModsSubPage
            {
                id: otherModsSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: false
            }
        }
    }



/*
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
    }*/
}
