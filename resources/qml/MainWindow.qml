import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import QtGraphicalEffects 1.15

Window {
    width: 1200
    height: 900
    title: "DoW Stats 2"
    color: "#eaeaea"
    flags: Qt.Window
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
                                //source: "qrc:/images/resources/images/ssstats.png"
                                source: "qrc:/images/resources/images/rank6_60.png"
                                anchors.rightMargin: 10
                                anchors.leftMargin: 10
                                anchors.bottomMargin: 10
                                anchors.topMargin: 10
                                Layout.minimumHeight: 60
                                Layout.minimumWidth: 60
                                Layout.margins: 0
                                Layout.rightMargin: 0
                                Layout.leftMargin: 0
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                fillMode: Image.PreserveAspectFit
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
                                text: qsTr("DoW Stats 2")
                                font.pixelSize: 25
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
                    text: "News"
                    pressedState: true
                    newsAvailable: newsPage.model.newsAvailable

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
                    text: "Events"
                    newsAvailable: eventsPage.model.newsAvailable

                   // property bool dataRequested: false

                    onClicked: {
                        /*if (!eventsButton.dataRequested)
                        {
                            _uiBackend.onRequestEvents();
                            eventsButton.dataRequested = true;

                        }*/

                        newsButton.pressedState = false;
                        infoButton.pressedState = false;
                        settingsButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        modsButton.pressedState = false
                    }
                }


                HeaderButton{
                    id: infoButton
                    text: "Information"

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
                    text: "Replay manager"
                    width: 160

                    onClicked: {
                        newsButton.pressedState = false;
                        eventsButton.pressedState = false;
                        infoButton.pressedState = false;
                        settingsButton.pressedState = false;
                        modsButton.pressedState = false
                    }
                }

                HeaderButton{
                    id: modsButton
                    text: "Mods"

                    onClicked: {
                        newsButton.pressedState = false;
                        eventsButton.pressedState = false;
                        infoButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        settingsButton.pressedState = false;

                    }
                }

                HeaderButton{
                    id: settingsButton
                    text: "Settings"

                    onClicked: {
                        newsButton.pressedState = false;
                        eventsButton.pressedState = false;
                        infoButton.pressedState = false;
                        replayManagerButton.pressedState = false;
                        modsButton.pressedState = false
                    }
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }
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

                    Switch {
                        id: trainingModeSwitch
                        text: "Ranked mode"
                        checked: _uiBackend.rankedModeState
                        anchors.fill: parent

                        opacity: hovered ? 1.0 : 0.8
                        font.pixelSize: 15

                        onCheckedChanged: {
                            _uiBackend.rankedModeState = checked;
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
            }
        }

        Botter{
            Layout.fillWidth: true
            Layout.minimumHeight: 50
        }
    }
}
