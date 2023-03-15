import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle
{
    id: root

    property var model
    property string text: ""

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
            text: root.text
            Layout.preferredWidth: 200
            font.pixelSize: 11
        }

        Label{
            id: progressLabel0;
            text: model.installProgress
            Layout.preferredWidth: 200
            font.pixelSize: 11
        }

        Rectangle{
            Layout.fillWidth: true
        }

        BlueButton{
            text: model.installedStatus ? "Reinstall" : "Install"
            enabled: !model.installInProcess

            onClicked: {
                model.installMod();
            }
        }

        BlueButton{
            text: "Delete"
            enabled: model.installedStatus

            onClicked: {
                model.uninstallMod();
            }
        }
    }
}
