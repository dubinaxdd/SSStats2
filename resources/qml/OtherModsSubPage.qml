import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle
{
    id: root
    property var model: _uiBackend.modsPage
    color: "#00000000"

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
            //color: "#00ff99"
            color: DowStatsStyle.itemColor//highlightItemColor//"#DCDCDC"

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                Label{
                    id: unlockerLabel
                    text: qsTr("Race unlocker:")
                    font.pixelSize: 11
                    Layout.preferredWidth: 200
                    color: DowStatsStyle.textColor
                }

                Label{
                    id: unlockerStateLabel
                    text: qsTr(model.unlockRacesStatus)
                    Layout.preferredWidth: 200
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }

                BlueButton{
                    id: unlockRacesButton
                    text: qsTr("Unlock races")
                    Layout.preferredWidth: 200 + columnLayout.spacing

                    onClicked: {
                        if (_uiBackend.gameLaunchState)
                            _uiBackend.soulstormLaunchedDialogVisible = true
                        else
                        {
                            model.unlockRaces();
                            enabled = false;
                        }
                    }
                }
            }
        }

        ModItem
        {
            model: root.model.russianFontsMod
            text: qsTr("Russian fonts:")
        }

        ModItem
        {
            model: root.model.cameraMod
            text: qsTr("Advanced camera mod:")
        }

        ModItem
        {
            model: root.model.gridHotkeysMod
            text: qsTr("QWERTY hotkeys:")
        }

        ModItem
        {
            model: root.model.transparentCameraTrapezoidMod
            text: qsTr("Transparent camera trapezoid:")
        }

        Rectangle{
            Layout.fillHeight: true
        }
    }
}
