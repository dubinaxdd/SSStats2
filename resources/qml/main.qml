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
    title: "SSStats2"
    color: "#00000000"
    visibility: Window.Maximized

    property var xMousePos
    property var yMousePos
    property var mouseAreaWidth
    property var mouseAreaHeight

    Connections{
        target: _uiBackend

        function onNoFogStateChanged(state){
            fullOverlay.settingsWindow.noFogSwitch.checkedState = state;
        }

        function onSendMousePress(){
            //Тут смотрим по какой кнопке пришолся клик, делаем это все "руками" тк оверлей игонирт события мыши и клавиатуры.
            //console.log(_uiBackend.ssWindowPositionX, _uiBackend.ssWindowPositionY, _uiBackend.ssWindowed, xMousePos, yMousePos, columnLayout3.x + statsHeader.expandButtonRectangleX, columnLayout3.y + statsHeader.expandButtonRectangleY, window.x, window.y, window.width, window.height , mouseAreaWidth, mouseAreaHeight);

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

            // Кнопка "Развернуть оверлей в игровой панели"
            if (xMousePos >= gamePanel.x + gamePanel.expandButtonRectangleX &&
                    xMousePos <= gamePanel.x + gamePanel.expandButtonRectangleX + gamePanel.expandButtonRectangle.width &&
                    yMousePos >= gamePanel.y + gamePanel.expandButtonRectangleY &&
                    yMousePos <= gamePanel.y + gamePanel.expandButtonRectangleY + gamePanel.expandButtonRectangle.height)
            {

                if(_uiBackend.gamePanel.gamePanelVisible)
                {
                    statsHeader.expandButtonRectangle.howeredState = true;
                    _uiBackend.expandKeyPressed();
                }
            }

            // Кнопка "Развернуть панель с рассами игроков"
            if (xMousePos >= gamePanel.x + gamePanel.expandPlayerRacesButtonX &&
                    xMousePos <= gamePanel.x + gamePanel.expandPlayerRacesButtonX + gamePanel.expandPlayerRacesButton.width &&
                    yMousePos >= gamePanel.y + gamePanel.expandPlayerRacesButtonY &&
                    yMousePos <= gamePanel.y + gamePanel.expandPlayerRacesButtonY + gamePanel.expandPlayerRacesButton.height)

            {

                if(_uiBackend.gamePanel.gamePanelVisible)
                {
                    gamePanel.expandPlayerRacesButton.howeredState = true;
                    _uiBackend.gamePanel.expandPlayerRacesButtonClick();
                }
            }

            // Кнопка "Свернуть колонку статистики"
            if (xMousePos >= columnLayout3.x + patyStatistic.x + patyStatistic.expandPatyStatisticButtonRectangle.x &&
                    xMousePos <= columnLayout3.x + patyStatistic.x + patyStatistic.expandPatyStatisticButtonRectangle.x + patyStatistic.expandPatyStatisticButtonRectangle.width &&
                    yMousePos >= columnLayout3.y + patyStatistic.y + patyStatistic.expandPatyStatisticButtonRectangle.y &&
                    yMousePos <= columnLayout3.y + patyStatistic.y + patyStatistic.expandPatyStatisticButtonRectangle.y + patyStatistic.expandPatyStatisticButtonRectangle.height)
            {
                if(!_uiBackend.gamePanel.gamePanelVisible)
                {
                    patyStatistic.expandPatyStatisticButtonRectangle.howeredState = true;
                    _uiBackend.expandPatyStatisticButtonClick();
                }
            }

            if(_uiBackend.expand){
                // Кнопка "Настройки" - отобразить окно с настройками
                if (xMousePos >= fullOverlay.x + fullOverlay.buttonSettings.x &&
                        xMousePos <= fullOverlay.x + fullOverlay.buttonSettings.x + fullOverlay.buttonSettings.width &&
                        yMousePos >= fullOverlay.y + fullOverlay.buttonSettings.y &&
                        yMousePos <= fullOverlay.y + fullOverlay.buttonSettings.y + fullOverlay.buttonSettings.height)
                {
                    fullOverlay.buttonInfo.pressedState = false;
                    fullOverlay.buttonSettings.pressedState = !fullOverlay.buttonSettings.pressedState;
                }

                // Кнопка "Информация" - отобразить окно с информацией
                if (xMousePos >= fullOverlay.x + fullOverlay.buttonInfo.x &&
                        xMousePos <= fullOverlay.x + fullOverlay.buttonInfo.x + fullOverlay.buttonInfo.width &&
                        yMousePos >= fullOverlay.y + fullOverlay.buttonInfo.y &&
                        yMousePos <= fullOverlay.y + fullOverlay.buttonInfo.y + fullOverlay.buttonInfo.height)
                {
                    fullOverlay.buttonSettings.pressedState = false;
                    fullOverlay.buttonInfo.pressedState = !fullOverlay.buttonInfo.pressedState;
                }

                if(fullOverlay.buttonSettings.pressedState){
                    // Переключатель "No Fog"
                    if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.noFogSwitch.x &&
                            xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.noFogSwitch.x + fullOverlay.settingsWindow.noFogSwitch.width &&
                            yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.noFogSwitch.y &&
                            yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.noFogSwitch.y + fullOverlay.settingsWindow.noFogSwitch.height)
                    {
                        //console.log("No Fog Switch Pressed: ", xMousePos, yMousePos);
                        fullOverlay.settingsWindow.noFogSwitch.checkedState = !fullOverlay.settingsWindow.noFogSwitch.checkedState;
                        _uiBackend.onSwitchNoFogStateChanged(fullOverlay.settingsWindow.noFogSwitch.checkedState);
                    }

                    // Переключатель "Test Switch"
                    if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.testSwitch.x &&
                            xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.testSwitch.x + fullOverlay.settingsWindow.testSwitch.width &&
                            yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.testSwitch.y &&
                            yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.testSwitch.y + fullOverlay.settingsWindow.testSwitch.height)
                    {
                        //console.log("Test Switch Pressed: ", xMousePos, yMousePos);
                        fullOverlay.settingsWindow.testSwitch.checkedState = !fullOverlay.settingsWindow.testSwitch.checkedState;
                    }
                }
            }
        }

        function onSendMouseMove(){
            xMousePos = _uiBackend.mousePositionX;
            yMousePos = _uiBackend.mousePositionY;
            mouseAreaWidth = _uiBackend.mouseAreaWidth;
            mouseAreaHeight = _uiBackend.mouseAreaHeight;

            // Костыль для работы с 4k мониторами, не спрашивайте почему так, все равно не скажу. Просто сс не может работать в большем разрешении чем 1920/1080

            if (!_uiBackend.ssWindowed && mouseAreaWidth !== 0 && mouseAreaHeight !== 0 && window.width >= 1920 && window.height >= 1080)
            {
                if (window.width !== mouseAreaWidth && window.height !== mouseAreaHeight)
                {
                    xMousePos = (window.width * xMousePos) / mouseAreaWidth;
                    yMousePos = (window.height * yMousePos) / mouseAreaHeight;
                }
            }

            // Это тоже кусок костыля, только для оконного режима игры
            if ( _uiBackend.ssWindowed && mouseAreaWidth > 1920 && mouseAreaHeight > 1080)
            {
                xMousePos = xMousePos/2;
                yMousePos = yMousePos/2;
            }

            if (_uiBackend.ssWindowed)
            {
                xMousePos = xMousePos - _uiBackend.ssWindowPositionX;
                yMousePos = yMousePos - _uiBackend.ssWindowPositionY;
            }

            // Отлавливаем все координаты курсора при перемещении
            //console.log(_uiBackend.ssWindowPositionX, _uiBackend.ssWindowPositionY, _uiBackend.ssWindowed, xMousePos, yMousePos, fullOverlay.x + fullOverlay.buttonSettings.x, fullOverlay.y + fullOverlay.buttonSettings.y, window.x, window.y, window.width, window.height , mouseAreaWidth, mouseAreaHeight);


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


            // Кнопка "Развернуть оверлей в игровой панели"
            if (xMousePos >= gamePanel.x + gamePanel.expandButtonRectangleX &&
                    xMousePos <= gamePanel.x + gamePanel.expandButtonRectangleX + gamePanel.expandButtonRectangle.width &&
                    yMousePos >= gamePanel.y + gamePanel.expandButtonRectangleY &&
                    yMousePos <= gamePanel.y + gamePanel.expandButtonRectangleY + gamePanel.expandButtonRectangle.height)

            {

                if(!gamePanel.expandButtonRectangle.howeredState)
                    gamePanel.expandButtonRectangle.howeredState = true;
            }
            else
            {
                if(gamePanel.expandButtonRectangle.howeredState)
                    gamePanel.expandButtonRectangle.howeredState = false;
            }


            // Кнопка "Развернуть панель с рассами игроков"
            if (xMousePos >= gamePanel.x + gamePanel.expandPlayerRacesButtonX &&
                    xMousePos <= gamePanel.x + gamePanel.expandPlayerRacesButtonX + gamePanel.expandPlayerRacesButton.width &&
                    yMousePos >= gamePanel.y + gamePanel.expandPlayerRacesButtonY &&
                    yMousePos <= gamePanel.y + gamePanel.expandPlayerRacesButtonY + gamePanel.expandPlayerRacesButton.height)

            {

                if(!gamePanel.expandPlayerRacesButton.howeredState)
                    gamePanel.expandPlayerRacesButton.howeredState = true;
            }
            else
            {
                if(gamePanel.expandPlayerRacesButton.howeredState)
                    gamePanel.expandPlayerRacesButton.howeredState = false;
            }

            // Кнопка "Свернуть колонку статистики"
            if (xMousePos >= columnLayout3.x + patyStatistic.x + patyStatistic.expandPatyStatisticButtonRectangle.x &&
                    xMousePos <= columnLayout3.x + patyStatistic.x + patyStatistic.expandPatyStatisticButtonRectangle.x + patyStatistic.expandPatyStatisticButtonRectangle.width &&
                    yMousePos >= columnLayout3.y + patyStatistic.y + patyStatistic.expandPatyStatisticButtonRectangle.y &&
                    yMousePos <= columnLayout3.y + patyStatistic.y + patyStatistic.expandPatyStatisticButtonRectangle.y + patyStatistic.expandPatyStatisticButtonRectangle.height)
            {

                if(!patyStatistic.expandPatyStatisticButtonRectangle.howeredState)
                    patyStatistic.expandPatyStatisticButtonRectangle.howeredState = true;
            }
            else
            {
                if(patyStatistic.expandPatyStatisticButtonRectangle.howeredState)
                    patyStatistic.expandPatyStatisticButtonRectangle.howeredState = false;
            }

            if(_uiBackend.expand){
                // Кнопка "Настройки"
                if (xMousePos >= fullOverlay.x + fullOverlay.buttonSettings.x &&
                        xMousePos <= fullOverlay.x + fullOverlay.buttonSettings.x + fullOverlay.buttonSettings.width &&
                        yMousePos >= fullOverlay.y + fullOverlay.buttonSettings.y &&
                        yMousePos <= fullOverlay.y + fullOverlay.buttonSettings.y + fullOverlay.buttonSettings.height)
                {
                    fullOverlay.buttonSettings.hoverState = true;
                } else {
                    fullOverlay.buttonSettings.hoverState = false;
                }

                // Кнопка "Информация"
                if (xMousePos >= fullOverlay.x + fullOverlay.buttonInfo.x &&
                        xMousePos <= fullOverlay.x + fullOverlay.buttonInfo.x + fullOverlay.buttonInfo.width &&
                        yMousePos >= fullOverlay.y + fullOverlay.buttonInfo.y &&
                        yMousePos <= fullOverlay.y + fullOverlay.buttonInfo.y + fullOverlay.buttonInfo.height)
                {
                    fullOverlay.buttonInfo.hoverState = true;
                } else {
                    fullOverlay.buttonInfo.hoverState = false;
                }

                if(fullOverlay.buttonSettings.pressedState){
                    // Переключатель "No Fog"
                    if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.noFogSwitch.x &&
                            xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.noFogSwitch.x + fullOverlay.settingsWindow.noFogSwitch.width &&
                            yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.noFogSwitch.y &&
                            yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.noFogSwitch.y + fullOverlay.settingsWindow.noFogSwitch.height)
                    {
                        //console.log("No Fog Switch Hovered: ", xMousePos, yMousePos);
                        fullOverlay.settingsWindow.noFogSwitch.hoverState = true;
                    } else {
                        fullOverlay.settingsWindow.noFogSwitch.hoverState = false;
                    }

                    // Переключатель "Test Switch"
                    if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.testSwitch.x &&
                            xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.settingsColumn.x + fullOverlay.settingsWindow.testSwitch.x + fullOverlay.settingsWindow.testSwitch.width &&
                            yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.testSwitch.y &&
                            yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.settingsColumn.y + fullOverlay.settingsWindow.testSwitch.y + fullOverlay.settingsWindow.testSwitch.height)
                    {
                        //console.log("Test Switch Hovered: ", xMousePos, yMousePos);
                        fullOverlay.settingsWindow.testSwitch.hoverState = true;
                    } else {
                        fullOverlay.settingsWindow.testSwitch.hoverState = false;
                    }
                }
            }
        }

        function onWindowTopmostChanged(){
            if (_uiBackend.topmost)
                window.flags =  Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground | Qt.WA_MSWindowsUseDirect3D | Qt.WA_ShowWithoutActivating
            else
                window.flags =  Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground | Qt.WA_MSWindowsUseDirect3D | Qt.WA_ShowWithoutActivating | Qt.WA_WState_Hidden
        }

        function onWindowedModeSeted()
        {
            window.flags =  /*Qt.WindowStaysOnTopHint |*/ Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground | Qt.WA_MSWindowsUseDirect3D | Qt.WA_ShowWithoutActivating
        }
    }

    SystemTrayIcon {
        id: trayIcon
        visible: true
        icon.source: "qrc:/icons/resources/icons/ssstats.ico"

        menu: Menu {
            MenuItem {
                text: qsTr("Exit")

                onTriggered: Qt.quit()
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

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent
            spacing: 0

            RowLayout {
                id: rowLayout
                spacing: 0

                Rectangle {
                    id: fullOverlayRectangle
                    width: 200
                    height: 200
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
                    width: 100
                    height: 100

                    StatsHeader{
                        id: statsHeader
                        width: 260
                        height: 60
                        Layout.minimumWidth: 260
                        Layout.minimumHeight: 60
                        Layout.fillHeight: true
                        visible: _uiBackend.headerVisible
                    }

                    PatyStatistic
                    {
                        id: patyStatistic
                        Layout.fillWidth: false
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        model: _uiBackend.statisticPanel
                        visible: _uiBackend.patyStatisticVisible
                    }

                    Rectangle {
                        id: rectangle2
                        width: 200
                        height: 200
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
