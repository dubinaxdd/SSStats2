import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: root
    property var model: _uiBackend.modsPage

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
            color: "#DCDCDC"

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                Label{
                    id: unlockerLabel
                    text: "Race unlocker:"
                    font.pixelSize: 11
                    Layout.preferredWidth: 200
                }

                Label{
                    id: unlockerStateLabel
                    text: model.unlockRacesStatus
                    Layout.preferredWidth: 200
                    font.pixelSize: 11
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

        ModItem
        {
            model: root.model.russianFontsMod
            text: "Russian fonts:"
        }

        ModItem
        {
            model: root.model.cameraMod
            text: "Advanced camera mod:"
        }

        ModItem
        {
            model: root.model.gridHotkeysMod
            text: "Grid hotkeys:"
        }

        ModItem
        {
            model: root.model.transparentCameraTrapezoidMod
            text: "Transparent camera trapezoid:"
        }

        Rectangle{
            Layout.fillHeight: true
        }
    }
}
