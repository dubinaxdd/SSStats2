import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import GlobalMouseProvider 1.0
import DowStatsStyle 1.0
import GameType 1.0

Item {
    id: window
    width: 640
    height: 480

    property real xMousePos
    property real yMousePos
    property real mouseAreaWidth
    property real mouseAreaHeight
    visible: _uiBackend.settingsPageModel.overlayVisible && !_uiBackend.settingsPageModel.legacyOverlayForDE && _uiBackend.gamePage.currentGameType  ===  GameType.DefinitiveEdition

    Component.onCompleted: {
        if (window.visible)
            GlobalMouseProvider.rootElement = windowRectangle
    }

    onVisibleChanged: GlobalMouseProvider.rootElement = windowRectangle

    //Component.onCompleted: GlobalMouseProvider.rootElement = windowRectangle

    Connections{
        target: window.visible ? _uiBackend : null

        function onSendMouseWheel(delta){
            GlobalMouseProvider.sendMouseWheel(delta);
        }

        function onSendMousePress(){
            GlobalMouseProvider.mouseClick();
        }

        function onSendMouseMove(){
            xMousePos = _uiBackend.mousePositionX;
            yMousePos = _uiBackend.mousePositionY;
            mouseAreaWidth = _uiBackend.mouseAreaWidth;
            mouseAreaHeight = _uiBackend.mouseAreaHeight;

            if (_uiBackend.ssWindowed)
            {
                xMousePos = xMousePos - _uiBackend.ssWindowPositionX;
                yMousePos = yMousePos - _uiBackend.ssWindowPositionY;
            }

            //xMousePos /= _uiBackend.devicePixelRatio
            //yMousePos /= _uiBackend.devicePixelRatio
            //mouseAreaWidth *= _uiBackend.devicePixelRatio
            //mouseAreaHeight *= _uiBackend.devicePixelRatio

            GlobalMouseProvider.mouseX = xMousePos;
            GlobalMouseProvider.mouseY = yMousePos;
        }
    }

    OverlayNotification{
        id: notification
        x: 50
        y: 50
    }

    Rectangle {
        id: windowRectangle
        color: "#00000000"
        anchors.fill: parent
        visible: _uiBackend.showClient

        Rectangle {
            id: backgroundRectangle
            color: "#80ffffff"
            anchors.fill: parent
            visible: _uiBackend.expand
        }

        GamePanel {
            id: gamePanel
            model: _uiBackend.gamePanel
            anchors.fill: parent
        }

        GamePanelSmall {
            id: gamePanelSmall
            model: _uiBackend.gamePanel
            anchors.fill: parent
        }

        GameLoadScreen
        {
            id: gameLoadScreen
            visible: _uiBackend.gameLoadScreenStatisticVisible
            anchors.fill: parent
        }

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent
            spacing: 0

            RowLayout {
                id: rowLayout
                spacing: 0

                Rectangle {
                    id: fullOverlayRectangle
                    width: 200 * _uiBackend.sizeModifer
                    height: 200 * _uiBackend.sizeModifer
                    color: "#00000000"
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    FullOverlay{
                        id: fullOverlay
                        visible: _uiBackend.expand
                        anchors.fill: parent
                    }
                }

                ColumnLayout {
                    id: columnLayout3

                    StatsHeader{
                        id: statsHeader
                        Layout.fillHeight: true
                        visible: _uiBackend.headerVisible
                        z: 3
                        showTrainingModeSwitch: true
                    }

                    RowLayout
                    {
                        spacing: 5 * _uiBackend.sizeModifer
                        visible: _uiBackend.headerVisible

                        Rectangle
                        {
                            Layout.alignment: Qt.AlignCenter
                            Layout.preferredHeight: 30 * _uiBackend.sizeModifer
                            Layout.minimumWidth: updateButton.visible ? (245 * _uiBackend.sizeModifer) : (280  * _uiBackend.sizeModifer)
                            Layout.maximumWidth: updateButton.visible ? (245 * _uiBackend.sizeModifer) : (280  * _uiBackend.sizeModifer)

                            Layout.fillWidth: true

                            radius: 10 * _uiBackend.sizeModifer
                            color: DowStatsStyle.backgroundColor


                            ColumnLayout
                            {
                                anchors.fill: parent

                                Label
                                {
                                    Layout.alignment: Qt.AlignCenter
                                    text: _uiBackend.currentModName + qsTr(" ladder")
                                    font.pixelSize: 15 * _uiBackend.sizeModifer
                                    color: DowStatsStyle.textColor
                                }
                            }
                        }

                        IconButton{
                            id: updateButton
                            sourceUrl: "qrc:/images/resources/images/update.svg"
                            containsMouse: updateButtonMouseArea.hovered

                            //width: 30 * _uiBackend.sizeModifer
                            //height: 30 * _uiBackend.sizeModifer
                            Layout.preferredHeight: 30 * _uiBackend.sizeModifer
                            Layout.preferredWidth: 30 * _uiBackend.sizeModifer

                            sizeModifer: _uiBackend.sizeModifer
                            visible: !_uiBackend.automatchState && _uiBackend.expandStatisticButtonVisible

                            GlobalMouseArea{
                                id: updateButtonMouseArea
                                anchors.fill: parent
                                onClicked: _uiBackend.statisticPanel.updateStatistic();
                            }
                        }
                    }

                    OverlayPlayersStatistic
                    {
                        id: patyStatistic
                        Layout.alignment: Qt.AlignTop
                        model: _uiBackend.statisticPanel
                        visible: _uiBackend.patyStatisticVisible
                        z: 2

                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
