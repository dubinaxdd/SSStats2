import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

Rectangle {
    id: root
    color: "#00000000"
    property var model: _uiBackend.balanceModPage

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 5

        visible: !root.model.haveAvilableMods

        Label{
            text: "No mods available."
            color: DowStatsStyle.textColor
            font.pixelSize: 20
            Layout.alignment: Qt.AlignCenter
        }
    }

    ColumnLayout
    {
        visible: root.model.haveAvilableMods

        anchors.fill: parent
        spacing: 5

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout{
                Layout.fillWidth: true
                Layout.fillHeight: true

                Label{
                    text: model.selectedModName
                    color: DowStatsStyle.textColor
                    font.pixelSize: 20
                }

                Label{
                    Layout.alignment: Qt.AlignVCenter
                    text: "Version: " + model.selectedModVersion
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                }


                RowLayout
                {
                    Rectangle{
                        radius:5

                        Layout.preferredHeight: 10
                        Layout.preferredWidth: 10

                        color:  model.selectedModIsInstalled ? "#00ff99" : "#ffa9a9"
                    }

                    Label{
                        Layout.alignment: Qt.AlignVCenter
                        text: model.selectedModIsInstalled ? "Status: Installed" : "Status: Not installed"
                        color: DowStatsStyle.textColor
                        font.pixelSize: 11
                    }
                }

                RowLayout
                {
                    Rectangle{
                        radius:5

                        Layout.preferredHeight: 10
                        Layout.preferredWidth: 10

                        color:  model.selectedModIsActual ? "#00ff99" : "#ffa9a9"
                    }

                    Label{
                        Layout.alignment: Qt.AlignVCenter
                        text: model.selectedModIsActual ? "This is latest version of this mod" : "This is an outdated version of this mod."
                        color: DowStatsStyle.textColor
                        font.pixelSize: 11
                    }
                }

                RowLayout
                {
                    Rectangle{
                        radius:5

                        Layout.preferredHeight: 10
                        Layout.preferredWidth: 10

                        color:  model.selectedModIsCurrent ? "#00ff99" : "#ffa9a9"
                    }

                    Label{
                        Layout.alignment: Qt.AlignVCenter
                        text: model.selectedModIsCurrent ? "This mod is active in the game" : "This mod is not active in the game"
                        color: DowStatsStyle.textColor
                        font.pixelSize: 11
                    }
                }

                RowLayout{

                    Layout.fillWidth: true;


                    BlueButton {
                        text: "Install"
                        enabled: !model.selectedModDownladingProcessed
                        onClicked:{

                            if (_uiBackend.ssLaunchState)
                                _uiBackend.soulstormLaunchedDialogVisible = true
                            else
                                model.downloadSelectedMod();
                        }

                        visible: !model.selectedModIsInstalled
                    }

                    BlueButton {
                        text: "Uninstall"
                        visible: model.selectedModIsInstalled

                        onClicked: {
                            if (_uiBackend.ssLaunchState)
                                _uiBackend.soulstormLaunchedDialogVisible = true
                            else
                                model.uninstallSelectedMod();
                        }

                    }

                    BlueButton {
                        text: "Activate in game"
                        Layout.preferredWidth: 100
                        visible: model.selectedModIsInstalled
                        enabled: !model.selectedModIsCurrent

                        onClicked:{
                            if (_uiBackend.ssLaunchState)
                                _uiBackend.soulstormLaunchedDialogVisible = true
                            else
                                model.activateSelectedModInGame();
                        }

                    }

                    Item {
                        Layout.fillWidth: true
                    }
                }

                Label{
                    text: model.downloadingProgress
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                }

                Item{
                    Layout.preferredHeight: 60
                }

                Label{
                    text: "Changelog"
                    color: DowStatsStyle.textColor
                    font.pixelSize: 14
                }

                Rectangle{
                    radius: 10
                    color: DowStatsStyle.alternateBackgroundColor
                    Layout.fillWidth: true
                    Layout.fillHeight: true


                    Flickable
                    {
                        id: flicable

                        anchors.fill: parent
                        anchors.margins: 15

                        contentHeight: changeLogTextArea.height
                        clip: true

                        ScrollBar.vertical: ScrollBar {
                            id: scrollBar
                            policy: ScrollBar.AlwaysOn
                        }

                        TextArea{
                            id:changeLogTextArea
                            width: parent.width - (scrollBar.visible ? scrollBar.width : 0)

                            wrapMode: Text.Wrap
                            color: DowStatsStyle.textColor
                            font.pixelSize: 11
                            //textFormat: Text.RichText
                            selectByMouse: true
                            readOnly: true

                            text: model.selectedChangeLog

                            onTextChanged: {
                                scrollBar.visible = changeLogTextArea.contentHeight > flicable.height
                            }
                        }

                        onHeightChanged: {
                            scrollBar.visible = changeLogTextArea.contentHeight > flicable.height
                        }
                    }
                }
            }

            Rectangle
            {
                Layout.fillHeight: true
                Layout.preferredWidth: 220
                radius: 10
                color: DowStatsStyle.alternateBackgroundColor

                ListView
                {
                    id: listView
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 5
                    clip: true
                    model: root.model

                    ScrollBar.vertical: ScrollBar {
                        id: scrollBarListViev
                        visible: listView.contentItem.height > listView.height
                        policy: ScrollBar.AlwaysOn
                    }

                    delegate: Rectangle{
                        width: listView.width - (scrollBarListViev.visible ? scrollBarListViev.width + 5 : 0)
                        height: 70
                        radius: 10
                        color: model.selected ? DowStatsStyle.selectionColor : delegateMouseArea.containsMouse ? DowStatsStyle.highlightItemColor : DowStatsStyle.backgroundColor

                        ColumnLayout
                        {
                            anchors.fill: parent
                            anchors.margins: 10

                            spacing: 3

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: model.uiName
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11
                            }

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: "Version: " + model.version
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11
                            }


                            RowLayout
                            {
                                Rectangle{
                                    radius:5

                                    Layout.preferredHeight: 10
                                    Layout.preferredWidth: 10

                                    color:  model.isInstalled ? "#00ff99" : "#ffa9a9"
                                }

                                Label{
                                    Layout.alignment: Qt.AlignVCenter
                                    text: model.isInstalled ? "Status: Installed" : "Status: Not installed"
                                    color: DowStatsStyle.textColor
                                    font.pixelSize: 11
                                }
                            }
                        }

                        MouseArea{
                            id:delegateMouseArea
                            anchors.fill: parent
                            hoverEnabled: true;

                            onClicked: root.model.slectItem(model.index);
                        }
                    }
                }
            }
        }

        Rectangle
        {
            radius: 10
            Layout.preferredHeight: 30
            color: extendedSettingsButtonMouseArea.containsMouse ? DowStatsStyle.highlightItemColor : DowStatsStyle.alternateBackgroundColor
            Layout.fillWidth: true

            clip: true

            RowLayout
            {
                anchors.fill: parent

                Layout.alignment: Qt.AlignCenter

                Item {
                    Layout.fillWidth: true
                }

                Label{
                    text: "Extended settings"
                    color: DowStatsStyle.textColor
                    font.pixelSize: 14
                }

                Item {

                    Layout.preferredWidth: 14
                    Layout.preferredHeight: 14

                    Image
                    {
                        id: image
                        anchors.margins: 3
                        source: extendedSettingsPannel.visible ? "qrc:/images/resources/images/arrow_down.svg" : "qrc:/images/resources/images/arrow_up.svg"
                        sourceSize.width: 14
                        sourceSize.height: 14
                    }

                    ColorOverlay{
                        anchors.fill: image
                        source:image
                        color: DowStatsStyle.textColor
                        antialiasing: true
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

            }

            MouseArea{
                id: extendedSettingsButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true

                onClicked: extendedSettingsPannel.visible = !extendedSettingsPannel.visible
            }

        }

        Rectangle
        {
            id: extendedSettingsPannel
            radius: 10
            Layout.preferredHeight: 180
            color: DowStatsStyle.alternateBackgroundColor
            Layout.fillWidth: true

            //Layout.topMargin: -15

            clip: true

            visible: false


            ColumnLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 5
                anchors.bottomMargin: 10

                StyledSwitch{
                    text: "Auto-install the latest version of DoW Stats Balance Mod"
                    checked: model.autoUpdateBalanceMod
                    onCheckedChanged: {

                        model.autoUpdateBalanceMod = checked;

                        if( checked)
                            model.downloadLatestMod();
                    }
                }

                StyledSwitch{
                    text: "Auto-uninstall the previous version of DoW Stats Balance Mod when installing the latest version"
                    checked: model.autoUninstallPreviousBalanceMod
                    onCheckedChanged: model.autoUninstallPreviousBalanceMod = checked
                }

                StyledSwitch{
                    id: templateProfilePathSwitch
                    text: "Use custom template profile path"
                    checked: model.useCustomTemplateProfilePath
                    onCheckedChanged: model.useCustomTemplateProfilePath = checked
                }

                Rectangle
                {
                    id: templateProfilePathPannel
                    radius: 10
                    Layout.preferredHeight: 70
                    color: DowStatsStyle.backgroundColor
                    Layout.fillWidth: true

                    RowLayout
                    {
                        id: rowLayout

                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.topMargin: 5
                        anchors.bottomMargin: 10

                        Layout.alignment: Qt.AlignTop


                        ColumnLayout
                        {
                            spacing: 3


                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: "Template profile path"
                                color: DowStatsStyle.textColor
                                font.pixelSize: 14


                            }

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: model.templateProfilePath
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11

                                clip: true

                                Layout.preferredWidth: rowLayout.width - rowLayout.spacing - (folderButton.visible ? folderButton.width : 0)
                            }

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: "Player color schemes and hotkeys will be copied after installing any mod version from the specified directory."
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11
                            }
                        }

                        IconButton
                        {
                            id: folderButton

                            sourceUrl: "qrc:/images/resources/images/folder.svg"
                            toolTipText: "Choise other folder"

                            visible: templateProfilePathSwitch.checked

                            onClicked: {
                                choiseFolderDialog.visible = true
                            }
                        }
                    }
                }
            }
        }
    }


    FolderDialog
    {
        id: choiseFolderDialog

        folder: "file:///" + model.templateProfilePath

        onAccepted: {
            model.choiseTemplateProfilePath(currentFolder);
            visible = false;
        }

        onRejected: {
            visible =  false;
        }
    }
}
