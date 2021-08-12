import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15



Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: "SSStats2"
    color: "#00000000"
    visibility: Window.Maximized

    Connections{
        target: _uiBackend

        function onSendMousePress(){
            var xMousePos = _uiBackend.mousePositionX;
            var yMousePos = _uiBackend.mousePositionY;
            var mouseAreaWidth = _uiBackend.mouseAreaWidth;
            var mouseAreaHeight = _uiBackend.mouseAreaHeight;

            //Костыль для работы с 4k мониторами, не спрашивайте почему так, все равно не скажу. Просто сс не может работать в большем разрешении чем 1920/1080

            if (!_uiBackend.ssWindowed && mouseAreaWidth !== 0 && mouseAreaHeight !== 0 && window.width >= 1920 && window.height >= 1080)
            {
                if (window.width !== mouseAreaWidth && window.height !== mouseAreaHeight)
                {
                    xMousePos = (window.width * xMousePos) / mouseAreaWidth;
                    yMousePos = (window.height * yMousePos) / mouseAreaHeight;
                }
            }

            //Это тоже кусок костыля, только для оконного режима игры
            if( _uiBackend.ssWindowed && mouseAreaWidth > 1920 && mouseAreaHeight > 1080)
            {
                xMousePos = xMousePos/2;
                yMousePos = yMousePos/2;
            }

            if(_uiBackend.ssWindowed)
            {
                xMousePos = xMousePos - _uiBackend.ssWindowPositionX;
                yMousePos = yMousePos - _uiBackend.ssWindowPositionY;
            }


            // ### КЛИК КУРСОРОМ (ГЛОБАЛЬНО) ###

            //Тут смотрим по какой кнопке пришолся клик, делаем это все "руками" тк оверлей игонирт события мыши и клавиатуры.
            //console.log(_uiBackend.ssWindowPositionX, _uiBackend.ssWindowPositionY, _uiBackend.ssWindowed, xMousePos, yMousePos, columnLayout3.x + statsHeader.expandButtonRectangleX, columnLayout3.y + statsHeader.expandButtonRectangleY, window.x, window.y, window.width, window.height , mouseAreaWidth, mouseAreaHeight);

            // Кнопка "Развернуть оверлей"
            if (xMousePos >= columnLayout3.x + statsHeader.expandButtonRectangleX &&
                    xMousePos <= columnLayout3.x + statsHeader.expandButtonRectangleX + statsHeader.expandButtonRectangleWidth &&
                    yMousePos >= columnLayout3.y + statsHeader.expandButtonRectangleY &&
                    yMousePos <= columnLayout3.y + statsHeader.expandButtonRectangleY + statsHeader.expandButtonRectangleHeight)
            {
                _uiBackend.expandKeyPressed();
            }

            // Кнопка "Развернуть оверлей в игровой панели"
            if (xMousePos >= gamePanel.x + gamePanel.expandButtonRectangleX &&
                    xMousePos <= gamePanel.x + gamePanel.expandButtonRectangleX + gamePanel.expandButtonRectangleWidth &&
                    yMousePos >= gamePanel.y + gamePanel.expandButtonRectangleY &&
                    yMousePos <= gamePanel.y + gamePanel.expandButtonRectangleY + gamePanel.expandButtonRectangleHeight)
            {
                _uiBackend.expandKeyPressed();
            }

            // Кнопка "Настройки" - отобразить окно с настройками
            if (xMousePos >= fullOverlay.x + fullOverlay.buttonSettingsX &&
                    xMousePos <= fullOverlay.x + fullOverlay.buttonSettingsX + fullOverlay.buttonSettingsWidth &&
                    yMousePos >= fullOverlay.y + fullOverlay.buttonSettingsY &&
                    yMousePos <= fullOverlay.y + fullOverlay.buttonSettingsY + fullOverlay.buttonSettingsHeight)
            {
                _uiBackend.buttonSettingsPressed();
            }

            // Кнопка "Информация" - отобразить окно с информацией
            if (xMousePos >= fullOverlay.x + fullOverlay.buttonInfoX &&
                    xMousePos <= fullOverlay.x + fullOverlay.buttonInfoX + fullOverlay.buttonInfoWidth &&
                    yMousePos >= fullOverlay.y + fullOverlay.buttonInfoY &&
                    yMousePos <= fullOverlay.y + fullOverlay.buttonInfoY + fullOverlay.buttonInfoHeight)
            {
                _uiBackend.buttonInfoPressed();
            }

            // Переключатель "No Fog"
            if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.noFogSwitchX &&
                    xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.noFogSwitchX + fullOverlay.settingsWindow.noFogSwitchWidth &&
                    yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.noFogSwitchY &&
                    yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.noFogSwitchY + fullOverlay.settingsWindow.noFogSwitchHeight)
            {
                //console.log("No Fog Switch Pressed: ", xMousePos, yMousePos);
                _uiBackend.switchNoFogPressed();
            }
        }

        // ### ПЕРЕМЕЩЕНИЕ КУРСОРА (ГЛОБАЛЬНО) ###
        function onSendMouseMove(){
            var xMousePos = _uiBackend.mousePositionX;
            var yMousePos = _uiBackend.mousePositionY;
            var mouseAreaWidth = _uiBackend.mouseAreaWidth;
            var mouseAreaHeight = _uiBackend.mouseAreaHeight;

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
            //console.log(_uiBackend.ssWindowPositionX, _uiBackend.ssWindowPositionY, _uiBackend.ssWindowed, xMousePos, yMousePos, fullOverlay.x + fullOverlay.buttonSettingsX, fullOverlay.y + fullOverlay.buttonSettingsY, window.x, window.y, window.width, window.height , mouseAreaWidth, mouseAreaHeight);

            // Кнопка "Настройки"
            if (xMousePos >= fullOverlay.x + fullOverlay.buttonSettingsX &&
                    xMousePos <= fullOverlay.x + fullOverlay.buttonSettingsX + fullOverlay.buttonSettingsWidth &&
                    yMousePos >= fullOverlay.y + fullOverlay.buttonSettingsY &&
                    yMousePos <= fullOverlay.y + fullOverlay.buttonSettingsY + fullOverlay.buttonSettingsHeight)
            {
                _uiBackend.buttonSettingsHoverStateChanged(true); // Попали в область кнопки "Настройки"
            } else {
                _uiBackend.buttonSettingsHoverStateChanged(false);
            }

            // Кнопка "Информация"
            if (xMousePos >= fullOverlay.x + fullOverlay.buttonInfoX &&
                    xMousePos <= fullOverlay.x + fullOverlay.buttonInfoX + fullOverlay.buttonInfoWidth &&
                    yMousePos >= fullOverlay.y + fullOverlay.buttonInfoY &&
                    yMousePos <= fullOverlay.y + fullOverlay.buttonInfoY + fullOverlay.buttonInfoHeight)
            {
                _uiBackend.buttonInfoHoverStateChanged(true); // Попали в область кнопки "Информация"
            } else {
                _uiBackend.buttonInfoHoverStateChanged(false);
            }

            // Переключатель "No Fog"
            if (xMousePos >= fullOverlay.x + fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.noFogSwitchX &&
                    xMousePos <= fullOverlay.x+ fullOverlay.mainContentContainerRectangle.x + fullOverlay.settingsWindow.noFogSwitchX + fullOverlay.settingsWindow.noFogSwitchWidth &&
                    yMousePos >= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.noFogSwitchY &&
                    yMousePos <= fullOverlay.y+ fullOverlay.mainContentContainerRectangle.y + fullOverlay.settingsWindow.noFogSwitchY + fullOverlay.settingsWindow.noFogSwitchHeight)
            {
                //console.log("No Fog Switch Hovered: ", xMousePos, yMousePos);
                _uiBackend.switchNoFogHoverStateChanged(true); // Попали в область переключателя "No Fog"
            } else {
                _uiBackend.switchNoFogHoverStateChanged(false);
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
                    }

                    PlayerStatistic
                    {
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    }

                    Rectangle {
                        id: rectangle2
                        width: 200
                        height: 200
                        color: "#00000000"
                        border.color: "#00000000"
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                    }
                }

            }
        }


    }
}
