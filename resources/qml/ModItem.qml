import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle
{
    id: root

    property var model
    property string text: ""

    Layout.preferredHeight: 50
    Layout.fillWidth: true
    color: DowStatsStyle.itemColor//highlightItemColor//"#DCDCDC"
    radius: 10

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 10

        Label{
            text: root.text
            Layout.preferredWidth: 200
            font.pixelSize: 11
            color: DowStatsStyle.textColor
        }

        Label{
            id: progressLabel0;
            text: qsTr(model.installProgress)
            Layout.preferredWidth: 200
            font.pixelSize: 11
            color: DowStatsStyle.textColor
        }

        Rectangle{
            Layout.fillWidth: true
        }

        BlueButton{
            text: model.installedStatus ? qsTr("Reinstall") : qsTr("Install")
            enabled: !model.installInProcess
            Layout.preferredWidth: 100

            onClicked: {
                if (_uiBackend.gameLaunchState)
                    _uiBackend.soulstormLaunchedDialogVisible = true
                else
                    model.installMod();
            }
        }

        BlueButton{
            text: qsTr("Delete")
            enabled: model.installedStatus
            Layout.preferredWidth: 100

            onClicked: {
                if (_uiBackend.gameLaunchState)
                    _uiBackend.soulstormLaunchedDialogVisible = true
                else
                    model.uninstallMod();
            }
        }
    }
}
