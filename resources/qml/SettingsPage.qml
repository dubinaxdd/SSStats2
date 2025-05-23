import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1
import DowStatsStyle 1.0

Rectangle {
    id: root
    opacity: 1
    color: DowStatsStyle.backgroundColor//"#ffffff"
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
    property var balanceModePageModel: _uiBackend.balanceModPage

    Connections
    {
        target: _uiBackend

        function onNoFogStateChanged()
        {
            noFogSwitch.checked = _uiBackend.noFogState;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onShowGamePannelPresetChanged()
        {
            showGamePannelSwitch.checked =  _uiBackend.gamePanel.showGamePannelPreset;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onSmallPannelActiveChanged()
        {
            smallPannelActiveSwitch.checked = _uiBackend.gamePanel.smallPannelActive;
        }
    }

    Flickable
    {
        clip: true

        ScrollBar.vertical: ScrollBar{
            visible: settingsContent.height > height
            policy: ScrollBar.AlwaysOn
        }

        anchors.margins: 15
        anchors.fill: parent

        contentHeight: settingsContent.height

        ColumnLayout
        {
            width: root.width - 30

            RowLayout
            {
                ColumnLayout
                {
                    id: settingsContent
                    Layout.preferredWidth: 350
                    Layout.fillHeight: true

                    StyledComboBox{
                        id: themeComboBox
                        Layout.preferredWidth: 255
                        model: [qsTr("Theme: Light"), qsTr("Theme: Dark"), qsTr("Theme: Pink"), qsTr("Theme: Blue")]
                        onCurrentIndexChanged:{
                            switch (currentIndex)
                            {
                                case(0): DowStatsStyle.setLightTheme(); break;
                                case(1): DowStatsStyle.setDarkTheme();  break;
                                case(2): DowStatsStyle.setPinkTheme();  break;
                                case(3): DowStatsStyle.setBlueTheme();  break;
                            }

                            root.model.currentTheme = themeComboBox.currentIndex;
                        }

                        Connections{
                            target: root.model
                            function onCurrentThemeChanged(){
                                themeComboBox.currentIndex = root.model.currentTheme
                            }
                        }
                    }

                    StyledComboBox{
                        id: languageComboBox
                        Layout.preferredWidth: 255
                        model: [qsTr("Language: System"), "Language: English", "Язык: Русский"]

                        onObjectSelected: root.model.language = currentIndex;

                        Connections{
                            target: root.model
                            function onLanguageChanged(){
                                languageComboBox.currentIndex = root.model.language
                            }
                        }


                    }

                    StyledSwitch{
                        text: qsTr("Autorun")
                        checked: model.autorun
                        onCheckedChanged: model.autorun = checked;
                    }

                    StyledSwitch{
                        text: qsTr("Show overlay")
                        checked: model.overlayVisible
                        onCheckedChanged: model.overlayVisible = checked;
                    }

                    StyledSwitch {
                        id: enableEventsSoundsEhenGameMinimized
                        text: qsTr("Enable events sounds when the game is minimized")
                        checked: model.enableEventsSoundWhenGameMinimized
                        onCheckedChanged: model.enableEventsSoundWhenGameMinimized = checked;
                    }

                    StyledSwitch {
                        id: enableEventsSoundsEhenGameMaximized
                        text: qsTr("Enable events sounds when the game is maximized")
                        checked: model.enableEventsSoundWhenGameMaximized
                        onCheckedChanged: model.enableEventsSoundWhenGameMaximized = checked;
                    }

                    StyledSwitch {
                        enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked
                        text: qsTr("Enable game load event sound")
                        checked: model.enableGameLoadEventSound
                        onCheckedChanged: model.enableGameLoadEventSound = checked;
                    }

                    StyledSwitch {
                        enabled: enableEventsSoundsEhenGameMinimized.checked || enableEventsSoundsEhenGameMaximized.checked
                        text: qsTr("Enable game start event sound")
                        checked: model.enableGameStartEventSound
                        onCheckedChanged: model.enableGameStartEventSound = checked;
                    }

                    RowLayout
                    {
                        Label
                        {
                            text: qsTr("Events volume:")
                            opacity: 0.8
                            font.pixelSize: 12
                            color: DowStatsStyle.textColor
                        }

                        Slider {
                            from: 0
                            value: model.volume
                            to: 100
                            opacity: hovered ? 1.0 : 0.8
                            onValueChanged: model.volume = value;
                        }
                    }

                    Rectangle
                    {
                        Layout.fillHeight: true
                    }
                }

                ColumnLayout
                {
                    StyledSwitch{
                        text: qsTr("Show overlay with Windows 7 support mode")
                        checked: model.win7SupportMode
                        onCheckedChanged: model.win7SupportMode = checked;
                    }

                    StyledSwitch{
                        text: qsTr("Launch Soulstorm in window")
                        checked: model.launchGameInWindow
                        onCheckedChanged: model.launchGameInWindow = checked;
                    }

                    StyledSwitch{
                        id: showGamePannelSwitch
                        text: qsTr("APM panel visible in game")
                        onCheckedChanged: _uiBackend.gamePanel.showGamePannelPreset = checked;
                    }

                    StyledSwitch{
                        id: smallPannelActiveSwitch
                        text: qsTr("Small APM panel in game")
                        onCheckedChanged: _uiBackend.gamePanel.smallPannelActive = checked;
                    }

                    StyledSwitch{
                        text: qsTr("Skip the intro video")
                        checked: model.skipIntroVideo
                        onCheckedChanged: model.skipIntroVideo = checked;
                    }

                    StyledSwitch{
                        id: noFogSwitch
                        text: qsTr("No FOG")
                        onCheckedChanged: _uiBackend.noFogState = checked;
                    }

                    StyledSwitch{
                        id: advertisingSwitch
                        text: qsTr("Auto ad sender")
                        checked: model.enableAdvertising
                        onCheckedChanged: model.enableAdvertising = checked;
                    }

                    Rectangle
                    {
                        Layout.fillHeight: true
                    }
                }
            }

            Rectangle
            {
                id: extendedSettingsPannel
                radius: 10
                Layout.preferredHeight: 220
                color: DowStatsStyle.alternateBackgroundColor
                Layout.fillWidth: true
                clip: true

                ColumnLayout
                {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.topMargin: 5
                    anchors.bottomMargin: 10

                    Label{
                        text: qsTr("DoW Stats Balance Mod settings")
                        color: DowStatsStyle.textColor
                        font.pixelSize: 14
                        Layout.margins: 5
                    }

                    StyledSwitch{
                        text: qsTr("Auto-install the latest version of DoW Stats Balance Mod")
                        checked: balanceModePageModel.autoUpdateBalanceMod
                        onCheckedChanged: {

                            balanceModePageModel.autoUpdateBalanceMod = checked;

                            if( checked)
                                balanceModePageModel.downloadLatestMod();
                        }
                    }

                    StyledSwitch{
                        text: qsTr("Auto-uninstall the previous version of DoW Stats Balance Mod when installing the latest version")
                        checked: balanceModePageModel.autoUninstallPreviousBalanceMod
                        onCheckedChanged: balanceModePageModel.autoUninstallPreviousBalanceMod = checked
                    }

                    StyledSwitch{
                        id: templateProfilePathSwitch
                        text: qsTr("Use custom template profile path")
                        checked: balanceModePageModel.useCustomTemplateProfilePath
                        onCheckedChanged: balanceModePageModel.useCustomTemplateProfilePath = checked
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
                                    text: qsTr("Template profile path")
                                    color: DowStatsStyle.textColor
                                    font.pixelSize: 14


                                }

                                Label{
                                    Layout.alignment: Qt.AlignVCenter
                                    text: balanceModePageModel.templateProfilePath
                                    color: DowStatsStyle.textColor
                                    font.pixelSize: 11

                                    clip: true

                                    Layout.preferredWidth: rowLayout.width - rowLayout.spacing - (folderButton.visible ? folderButton.width : 0)
                                }

                                Label{
                                    Layout.alignment: Qt.AlignVCenter
                                    text: qsTr("Player color schemes and hotkeys will be copied after installing any mod version from the specified directory.")
                                    color: DowStatsStyle.textColor
                                    font.pixelSize: 11
                                }
                            }

                            IconButton
                            {
                                id: folderButton

                                sourceUrl: "qrc:/images/resources/images/folder.svg"
                                toolTipText: qsTr("Choise other folder")

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
