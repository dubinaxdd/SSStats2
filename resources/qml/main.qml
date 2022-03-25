import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: "DowStats2Overlay"
    color: "#00000000"
    visibility: Window.Maximized

    property var xMousePos
    property var yMousePos
    property var mouseAreaWidth
    property var mouseAreaHeight

    Connections{
        target: _uiBackend

        function onSendMouseWheel(delta)
        {
            patyStatistic.mouseWheel(delta);
        }


        function onSendMousePress(){
            //Тут смотрим по какой кнопке пришолся клик, делаем это все "руками" тк оверлей игонирт события мыши и клавиатуры.
          /*  console.log("Windowed mode: ", _uiBackend.ssWindowed, "xMousePos: ", xMousePos, "yMousePos: ", yMousePos,
                        "Button position X: ", columnLayout3.x + statsHeader.expandButtonRectangle.x,
                        "Button position Y: ",columnLayout3.y + statsHeader.expandButtonRectangle.y,
                        "Window position X:", window.x, "Window position Y:", window.y,
                        "Window wifth:",window.width, "Window height:", window.height ,
                        "mouseAreaWidth:", mouseAreaWidth, "mouseAreaHeight:", mouseAreaHeight);*/

            // Кнопка "Развернуть оверлей"
            if (xMousePos >= columnLayout3.x + statsHeader.expandButtonRectangle.x &&
                    xMousePos <= columnLayout3.x + statsHeader.expandButtonRectangle.x + statsHeader.expandButtonRectangle.width &&
                    yMousePos >= columnLayout3.y + statsHeader.expandButtonRectangle.y &&
                    yMousePos <= columnLayout3.y + statsHeader.expandButtonRectangle.y + statsHeader.expandButtonRectangle.height)
            {
                if(_uiBackend.headerVisible)
                {
                    _uiBackend.expandKeyPressed();
                    statsHeader.expandButtonRectangle.howeredState = true;
                }
            }


            patyStatistic.mouseClick(xMousePos - columnLayout3.x - patyStatistic.x, yMousePos - columnLayout3.y - patyStatistic.y);


            if(_uiBackend.gamePanel.showGamePannelPreset)
            {
                if (!_uiBackend.gamePanel.smallPannelActive )
                {
                    gamePanel.mouseClick(xMousePos, yMousePos);
                }
                else
                {
                    gamePanelSmall.mouseClick(xMousePos, yMousePos);
                }
            }

            if(_uiBackend.expand){
                fullOverlay.mouseClick(xMousePos - fullOverlay.x , yMousePos - fullOverlay.y );
            }
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

            // Отлавливаем все координаты курсора при перемещении
            //console.log("Позиция курсора Х: ", _uiBackend.ssWindowPositionX, "Позиция курсора У: ",_uiBackend.ssWindowPositionY,
            //            "Оконный режим: ", _uiBackend.ssWindowed, "xMousePos: ", xMousePos, "yMousePos: ", yMousePos, fullOverlay.x + fullOverlay.buttonSettings.x, fullOverlay.y + fullOverlay.buttonSettings.y, window.x, window.y, window.width, window.height , mouseAreaWidth, mouseAreaHeight);


            // Кнопка "Развернуть оверлей"
            if (xMousePos >= columnLayout3.x + statsHeader.expandButtonRectangle.x &&
                    xMousePos <= columnLayout3.x + statsHeader.expandButtonRectangle.x + statsHeader.expandButtonRectangle.width &&
                    yMousePos >= columnLayout3.y + statsHeader.expandButtonRectangle.y &&
                    yMousePos <= columnLayout3.y + statsHeader.expandButtonRectangle.y + statsHeader.expandButtonRectangle.height)
            {
                if(!statsHeader.expandButtonRectangle.howeredState)
                    statsHeader.expandButtonRectangle.howeredState = true;
            }
            else
            {
                if(statsHeader.expandButtonRectangle.howeredState)
                    statsHeader.expandButtonRectangle.howeredState = false;
            }

            if (!_uiBackend.gamePanel.smallPannelActive)
            {
                gamePanel.mouseHover(xMousePos, yMousePos);
            }
            else
            {
                gamePanelSmall.mouseHover(xMousePos, yMousePos);
            }

            patyStatistic.mouseHover(xMousePos - patyStatistic.x - columnLayout3.x, yMousePos - patyStatistic.y - columnLayout3.y );

            if(_uiBackend.expand)
            {
                fullOverlay.mouseHover(xMousePos - fullOverlay.x, yMousePos - fullOverlay.y );
            }
        }

        function onWindowTopmostChanged(){
            if (_uiBackend.topmost)
                window.flags =  Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Tool |Qt.Popup | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating
            else
                window.flags =  Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating | Qt.WA_WState_Hidden
        }

        function onWindowedModeSeted()
        {
            window.flags =  /*Qt.WindowStaysOnTopHint |*/ Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground /*| Qt.WA_MSWindowsUseDirect3D*/ | Qt.WA_ShowWithoutActivating
        }
    }

    SystemTrayIcon {
        id: trayIcon
        visible: true
        icon.source: "qrc:/icons/resources/icons/ssstats.ico"

        menu: Menu {
            MenuItem {
                text: qsTr("Launch Soulstorm with Windows7 support mode")

                onTriggered: _uiBackend.onLaunchSoulstormWithSupportMode();
            }

            MenuItem {
                text: qsTr("Exit")

                onTriggered: _uiBackend.onExit();
            }
        }
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
                    }

                    PlayersStatistic
                    {
                        id: patyStatistic
                        Layout.alignment: Qt.AlignTop
                        model: _uiBackend.statisticPanel
                        visible: _uiBackend.patyStatisticVisible
                        z: 2
                    }

                    Rectangle {
                        id: rectangle2
                        width: 0
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        radius: 0
                        border.color: "#00000000"
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
