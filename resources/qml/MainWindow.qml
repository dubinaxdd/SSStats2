import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

Window {
    id: root
    width: 1200
    height: 700
    title: "DoW Stats Client"
    color: DowStatsStyle.alternateBackgroundColor//"#eaeaea"
    flags: Qt.Window //| Qt.FramelessWindowHint
    visibility: Window.Maximized

    minimumWidth: 1200
    minimumHeight: 700

    Connections
    {
        target: _uiBackend

        function onRankedModeStateChanged()
        {
            trainingModeSwitch.checked = _uiBackend.rankedModeState;
        }
    }

    onClosing: {
        root.hide();
    }

    PopupDialog{
        id: owerwriteProfileDialog
        visible: _uiBackend.balanceModPage.profileCopyModeRequestMessageVisible
        okButtonVisible: false
        text: qsTr("The profile folder of the installed mod already exists. Overwrite the hotkeys and army colors using Template Profile Path?")
        onClickedYes:{
            _uiBackend.balanceModPage.profileCopyModeRequestMessageVisible = false;
            _uiBackend.balanceModPage.continueModInstallation(true);
        }

        onClickedNo:{
            _uiBackend.balanceModPage.profileCopyModeRequestMessageVisible = false;
            _uiBackend.balanceModPage.continueModInstallation(false);
        }
    }

    PopupDialog{
        id: latesBalanceModNotInstalledDialog
        visible: _uiBackend.latesBalanceModNotInstalledDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("The latest DoW Stats Balance Mod version is not installed!") + "\n" +
              qsTr("You can install the latest DoW Stats Balance Mod  version from Mods page or select other mod from the bottom panel.")

        onClickedOk:{
            _uiBackend.latesBalanceModNotInstalledDialogVisible = false;
        }
    }

    PopupDialog{
        id: soulstormNotInstalledDialog
        visible: _uiBackend.ssNotInstalledDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("Dawn of War - Soulstorm is not installed, or installed incorrectly!") + "\n" +
              qsTr("You can install Dawn of War - Soulstorm from Steam store.") + "\n" +
              qsTr("Dow Stats Client only works with the Steam version of the game.") + "\n" +
              qsTr("Restart Dow Stats Client after installing the game.") + "\n" +
              qsTr("The first launch of the game must be done through Steam.")

        onClickedOk:{
            _uiBackend.ssNotInstalledDialogVisible = false;
        }
    }

    PopupDialog{
        id: steamNotInstalledDialog
        visible: _uiBackend.steamNotInstalledDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("Steam is not installed!") + "\n" +
              qsTr("Install Steam and try again.") + "\n" +
              qsTr("Restart Dow Stats Client after installing Steam.")

        onClickedOk:{
            _uiBackend.steamNotInstalledDialogVisible = false;
        }
    }

    PopupDialog{
        id: softwareUseBanDialog
        visible: _uiBackend.softwareUseBanDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("Your account is permanently banned. You can use other services.") + (_uiBackend.softwareUseBanReason === "" ? "" : "\n" + qsTr("Reason: ") + _uiBackend.softwareUseBanReason)

        onClickedOk:{
            Qt.openUrlExternally("https://dowonline.ru");
            _uiBackend.softwareUseBanDialogVisible = false;
            _uiBackend.onExit();
        }
    }

    PopupDialog{
        id: soulstormLaunchedDialog
        visible: _uiBackend.soulstormLaunchedDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("The game is currently running, please close the game and try again.")

        onClickedOk:{
            _uiBackend.soulstormLaunchedDialogVisible = false;
        }
    }

    PopupDialog{
        id: balanceModInstallProcessedDialog
        visible: _uiBackend.balanceModInstallProcessedDialogVisible
        yesButtonVisible: false
        noButtonVisible: false
        text: qsTr("Dow Stats Balance Mod is being installed, please wait until the installation is complete and try again.")

        onClickedOk:{
            _uiBackend.balanceModInstallProcessedDialogVisible = false;
        }
    }

    NotificationList{
        x: mainContentContainerRectangle.x
        y: mainContentContainerRectangle.y + header.height
        z: 2

        height: mainContentContainerRectangle.height
    }

    UniqueOnlineStatistic
    {
        x: parent.width - width - 5
        y: header.height + 5
        z: 3
        visible: onlineCounter.pressedState
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        spacing: 0

        Rectangle {
            id: header
            color: "#333333"
            Layout.fillWidth: true
            Layout.minimumHeight: 60

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#333333"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            Layout.maximumHeight: 60
            Layout.maximumWidth: 26099
            Layout.fillHeight: false

            RowLayout
            {
                anchors.fill: parent

                spacing: 0

                Rectangle {
                    id: headerRectangle
                    width: 280
                    height: 60
                    color: "#333333"
                    Layout.minimumWidth: 220
                    Layout.minimumHeight: 60
                    Layout.maximumHeight: 60
                    Layout.maximumWidth: 220

                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#333333"
                        }

                        GradientStop {
                            position: 1
                            color: "#000000"
                            //color: "#48cc11"
                        }
                    }

                    Layout.fillHeight: false

                    RowLayout {
                        id: rowLayout2
                        anchors.fill: parent
                        spacing: 0

                        Rectangle {
                            id: headerTM
                            width: 200
                            height: 200
                            color: "#00000000"
                            border.color: "#00000000"
                            Layout.maximumHeight: 60
                            Layout.maximumWidth: 60
                            Layout.minimumHeight: 60
                            Layout.minimumWidth: 60
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Image {
                                id: image
                                anchors.fill: parent

                                source: "qrc:/images/resources/images/Rank8.svg"
                                anchors.rightMargin: 10
                                anchors.leftMargin: 10
                                anchors.bottomMargin: 10
                                anchors.topMargin: 10
                                fillMode: Image.PreserveAspectFit

                                sourceSize.width: 60
                                sourceSize.height: 60
                            }
                        }

                        ColumnLayout {
                            id: columnLayout2
                            width: 100
                            height: 100
                            Layout.fillWidth: false
                            Layout.fillHeight: false

                            Label {
                                id: label
                                color: "#ffffff"
                                text: "DoW Stats Client"
                                font.pixelSize: 20
                            }

                            Label {
                                id: versionLabel
                                color: "#999999"
                                text: "v" + _uiBackend.ssStatsVersion
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                font.pixelSize: 12
                            }
                        }

                        Rectangle{
                            Layout.fillWidth: true
                        }
                    }
                }

                HeaderButton{
                    id: newsButton
                    text: qsTr("DoW Stats News")
                    pressedState: true
                    newsAvailable: newsPage.model.newsAvailable

                    //width: 170

                    onClicked: {
                        eventsButton.pressedState = false;
                        infoButton.pressedState = false;
                        settingsButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        modsButton.pressedState = false

                    }
                }

                HeaderButton{
                    id: eventsButton
                    text: qsTr("Community News")
                    newsAvailable: eventsPage.model.newsAvailable

                    //width: 170

                    onClicked: {
                        newsButton.pressedState = false;
                        infoButton.pressedState = false;
                        settingsButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        modsButton.pressedState = false
                    }
                }


                HeaderButton{
                    id: infoButton
                    text: qsTr("Information")

                    onClicked: {
                        newsButton.pressedState = false;
                        eventsButton.pressedState = false;
                        settingsButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        modsButton.pressedState = false
                    }
                }

                HeaderButton{
                    id: replayManagerButton
                    text: qsTr("Replay Manager")
                    enabled: _uiBackend.soulstormIsInstalled

                    onClicked: {
                        if (_uiBackend.soulstormIsInstalled)
                        {
                            newsButton.pressedState = false;
                            eventsButton.pressedState = false;
                            infoButton.pressedState = false;
                            settingsButton.pressedState = false;
                            modsButton.pressedState = false
                        }
                        else
                        {
                            pressedState = false;
                            _uiBackend.ssNotInstalledDialogVisible = true
                        }
                    }
                }

                HeaderButton{
                    id: modsButton
                    text: qsTr("Mods")
                    newsAvailable: _uiBackend.mapManagerPage.updatesAvailable
                    enabled: _uiBackend.soulstormIsInstalled

                    onClicked: {
                        if (_uiBackend.soulstormIsInstalled)
                        {
                            newsButton.pressedState = false;
                            eventsButton.pressedState = false;
                            infoButton.pressedState = false;
                            replayManagerButton.pressedState = false;
                            settingsButton.pressedState = false;
                        }
                        else
                        {
                            pressedState = false;
                            _uiBackend.ssNotInstalledDialogVisible = true
                        }

                    }
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }

                Rectangle
                {
                    id: settingsButton
                    width: 30
                    height: 30
                    color:"#00000000"

                    Layout.rightMargin: 15
                    property bool pressedState: false

                    MouseArea
                    {
                        id: settingsButtonMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        cursorShape: Qt.PointingHandCursor

                        onClicked:
                        {
                            settingsButton.pressedState = true;
                            newsButton.pressedState = false;
                            eventsButton.pressedState = false;
                            infoButton.pressedState = false;
                            replayManagerButton.pressedState = false;
                            modsButton.pressedState = false
                        }

                        ToolTip.visible: containsMouse
                        ToolTip.delay: 1000
                        ToolTip.text: qsTr("Settings")
                    }

                    Image {

                       anchors.fill: parent
                       source: settingsButtonMouseArea.containsMouse || settingsButton.pressedState ? "qrc:/images/resources/images/gear.svg" : "qrc:/images/resources/images/gear2.svg"
                       sourceSize.width: 30
                       sourceSize.height: 30
                    }


                }

                OnlineCounter{
                    id: onlineCounter
                }

               /* Rectangle
                {
                    width: 25
                    height: 25
                    color:"#00000000"

                    Image {
                       anchors.fill: parent
                       source: "qrc:/images/resources/images/man_user.svg"
                       sourceSize.width: 25
                       sourceSize.height: 25
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        hoverEnabled: true

                        ToolTip.visible: containsMouse
                        ToolTip.delay: 1000
                        ToolTip.text: ("Online DowStats users")
                    }
                }

                Label
                {
                    text: _uiBackend.onlineCount
                    font.pixelSize: 15
                    color: "#ffffff"
                    Layout.rightMargin: 15
                }*/
            }
        }

        RowLayout
        {
            spacing: 0

            Rectangle {
                id: mainContentContainerRectangle
                width: 200
                height: 200
                opacity: 1
                color: "#00000000"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.rightMargin: 10
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.topMargin: 5
                Layout.preferredHeight: 0
                Layout.preferredWidth: 0
                Layout.fillWidth: true
                Layout.fillHeight: true


                MessagesPage{
                    id: newsPage
                    visible: newsButton.pressedState
                    model: _uiBackend.newsPage
                    anchors.fill: parent
                }

                MessagesPage{
                    id: eventsPage

                    visible: eventsButton.pressedState
                    model: _uiBackend.eventsPage
                    anchors.fill: parent
                }

                InfoWindow {
                    id: infoWindow
                    visible: infoButton.pressedState
                    anchors.fill: parent
                }

                ReplayManagerPage {
                    id: replayManagerPage
                    visible: replayManagerButton.pressedState
                    anchors.fill: parent

                }

                ModsPage{
                    id: modsPage
                    visible: modsButton.pressedState
                    anchors.fill: parent
                }

                SettingsPage {
                    id: settingsPage
                    visible: settingsButton.pressedState
                    anchors.fill: parent
                }
            }

            ColumnLayout
            {
                id: patyStatisticColumnLayout

                visible: _uiBackend.statisticPanel.curentPlayerStatsItem.itemVisible && _uiBackend.statisticPanel.curentPlayerStatsItem.playerName !== ""

                Rectangle
                {
                    Layout.maximumWidth: 280
                    Layout.maximumHeight: 40
                    Layout.minimumWidth: 280
                    Layout.minimumHeight: 40
                    Layout.rightMargin: 5
                    height: 40
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.fillHeight: false
                    Layout.fillWidth: true
                   // color: "#ffffff"

                    color: trainingModeSwitch.checked ? "#00ff99" : "#ffa9a9";
                    radius: 10

                    Rectangle{
                        width: parent.width
                        height: 10

                        color: trainingModeSwitch.checked ? "#00ff99" : "#ffa9a9";
                    }


                    StyledSwitch {
                        id: trainingModeSwitch
                        text: qsTr("Ranked mode")
                        checked: _uiBackend.rankedModeState
                        fontPxSize: 15

                        textColor: "#26282a"
                        backgroundCheckedColor: "#A9A9A9"
                        backgroundUncheckedColor: "#c8c8c8"
                        indicatorColor: "#FFFFFF"

                        anchors.fill: parent
                        anchors.leftMargin: 10

                        onCheckedChanged: {
                            _uiBackend.rankedModeState = checked;
                        }
                    }

                }

                RowLayout
                {
                    spacing: 5

                    Rectangle
                    {
                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredHeight: 30
                        Layout.minimumWidth: _uiBackend.ssLaunchState ? 245 : 280
                        Layout.maximumWidth: _uiBackend.ssLaunchState ? 245 : 280
                        radius: 10
                        color: DowStatsStyle.backgroundColor

                        ColumnLayout
                        {
                            anchors.fill: parent

                            Label
                            {
                                Layout.alignment: Qt.AlignCenter
                                text: _uiBackend.currentModName + qsTr(" ladder")
                                font.pixelSize: 15
                                color: DowStatsStyle.textColor
                            }
                        }
                    }

                    IconButton{
                        sourceUrl: "qrc:/images/resources/images/update.svg"
                        toolTipText: qsTr("Update")
                        Layout.rightMargin: 5

                        visible: _uiBackend.ssLaunchState

                        Layout.preferredHeight: 30
                        Layout.preferredWidth: 30

                        onClicked: {
                            _uiBackend.statisticPanel.updateStatistic();
                        }
                    }
                }

                PlayersStatistic
                {
                    id: patyStatistic
                    Layout.alignment: Qt.AlignTop
                    model: _uiBackend.statisticPanel
                    Layout.fillHeight: true
                    Layout.rightMargin: 5
                    Layout.leftMargin: 0
/*
                    onVisibleChanged: {
                         patyStatisticColumnLayout.visible = patyStatistic.visible;
                    }*/
                }

                OnlineStatisticPanel{
                    Layout.preferredWidth: 280
                    Layout.preferredHeight: 127 - 20
                    Layout.rightMargin: 5
                    Layout.bottomMargin: 10
                }
            }
        }

        Botter{
            Layout.fillWidth: true
            Layout.minimumHeight: 50
        }
    }
}
