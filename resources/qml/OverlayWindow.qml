import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import GlobalMouseProvider 1.0

Window {
    id: window
    width: 640
    height: 480
    visible: _uiBackend.settingsPageModel.overlayVisible
    title: "DowStats2Overlay"
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

        function onSendMouseWheel(delta)
        {
            patyStatistic.mouseWheel(delta);
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


    Notification{
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

                    Rectangle {
                        id: expandPatyStatisticButtonRectangle

                        property bool howeredState: expandButton.howered

                        property Gradient grLight: Gradient {
                            GradientStop {
                                position: 0
                                color: "#428bca"
                            }

                            GradientStop {
                                position: 1
                                color: "#265a88"
                            }
                        }

                        property Gradient grDark: Gradient {
                            GradientStop {
                                position: 0
                                color: "#337ab7"
                            }

                            GradientStop {
                                position: 1
                                color: "#245580"

                            }
                        }

                        Layout.maximumWidth: 280 * _uiBackend.sizeModifer
                        Layout.minimumWidth: 280 * _uiBackend.sizeModifer
                        width: 280 * _uiBackend.sizeModifer

                        Layout.maximumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                        Layout.minimumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                        height: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0

                        radius: 5 * _uiBackend.sizeModifer

                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        //visible: !_uiBackend.expand
                        gradient: howeredState ? grDark : grLight

                        Image {
                            id: image

                            visible: !_uiBackend.expand
                            width: 95 * _uiBackend.sizeModifer
                            height: 10 * _uiBackend.sizeModifer
                            anchors.verticalCenter: parent.verticalCenter
                            source: "qrc:/images/resources/images/expandDots.png"
                            anchors.horizontalCenter: parent.horizontalCenter
                            fillMode: Image.PreserveAspectFit
                        }

                        GlobalMouseArea{
                            id: expandButton
                            anchors.fill: parent

                            onClicked: {
                                _uiBackend.expandPatyStatisticButtonClick();
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
