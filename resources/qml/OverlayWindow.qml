import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import GlobalMouseProvider 1.0
import DowStatsStyle 1.0

Window {
    id: window
    width: 640
    height: 480
    visible: _uiBackend.settingsPageModel.overlayVisible
    title: "DowStatsClientOverlay"
    color: "#00000000"
    visibility: Window.Maximized

    property real xMousePos
    property real yMousePos
    property real mouseAreaWidth
    property real mouseAreaHeight

    Component.onCompleted: GlobalMouseProvider.rootElement = windowRectangle

    onVisibilityChanged: {
        patyStatistic.resetScrollView();
    }

    Connections{
        target: _uiBackend

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

            xMousePos /= _uiBackend.devicePixelRatio
            yMousePos /= _uiBackend.devicePixelRatio

            GlobalMouseProvider.mouseX = xMousePos;
            GlobalMouseProvider.mouseY = yMousePos;
        }

        function onWindowTopmostChanged(){
            if (_uiBackend.topmost)
            {
                //window.showMaximized();
               // window.setWidth(mouseAreaWidth);
               // window.setHeight(mouseAreaHeight);
               // window.showMaximized();
                window.flags =  Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Tool |Qt.Popup | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating
            }
            else
            {
                window.flags =  Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating | Qt.WA_WState_Hidden
                window.setWidth(0);
                window.setHeight(0);
            }
        }

        function onWindowedModeSeted()
        {
            window.flags =  /*Qt.WindowStaysOnTopHint |*/ Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating
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
                            Layout.minimumWidth: 245 * _uiBackend.sizeModifer
                            Layout.maximumWidth: 245 * _uiBackend.sizeModifer
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
                            sourceUrl: "qrc:/images/resources/images/update.svg"
                            toolTipText: qsTr("Update")
                            containsMouse: updateButtonMouseArea.hovered

                            Layout.preferredHeight: 30 * _uiBackend.sizeModifer
                            Layout.preferredWidth: 30 * _uiBackend.sizeModifer
                            sizeModifer: _uiBackend.sizeModifer

                            GlobalMouseArea{

                                id: updateButtonMouseArea
                                anchors.fill: parent

                                onClicked: {
                                    _uiBackend.statisticPanel.updateStatistic();
                                }
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
