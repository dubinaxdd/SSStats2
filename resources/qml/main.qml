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

            //Тут смотрим по какой кнопке пришолся клик, делаем это все "руками" тк оверлей игонирт события мыши и клавиатуры.

            //console.log(xMousePos, xMousePos, columnLayout3.x + statsHeader.expandButtonRectangleX, columnLayout3.y + statsHeader.expandButtonRectangleY);

            //Рнопка "Развернуть оверлей"
            if (xMousePos >= columnLayout3.x + statsHeader.expandButtonRectangleX &&
                    xMousePos <= columnLayout3.x + statsHeader.expandButtonRectangleX + statsHeader.expandButtonRectangleWidth &&
                    yMousePos >= columnLayout3.y + statsHeader.expandButtonRectangleY &&
                    yMousePos <= columnLayout3.y + statsHeader.expandButtonRectangleY + statsHeader.expandButtonRectangleHeight)
            {
                _uiBackend.expandKeyPressed();
            }
        }

        function onWindowTopmostChanged(){
            if (_uiBackend.topmost)
                window.flags =  Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput | Qt.WindowFullScreen | Qt.WA_TranslucentBackground | Qt.WA_MSWindowsUseDirect3D | Qt.WA_ShowWithoutActivating
            else
                window.flags =  Qt.FramelessWindowHint | Qt.Tool | Qt.Window | Qt.WindowTransparentForInput /*| Qt.WindowFullScreen*/ | Qt.WA_TranslucentBackground | Qt.WA_MSWindowsUseDirect3D | Qt.WA_ShowWithoutActivating | Qt.WA_WState_Hidden
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

                    Rectangle {
                        id: rectangle1
                        width: 200
                        height: 200
                        color: "#ffffff"
                        Layout.maximumHeight: 100
                        Layout.maximumWidth: 200
                        Layout.fillWidth: false
                        Layout.fillHeight: true
                        //anchors.horizontalCenter: parent.horizontalCenter

                        ColumnLayout {
                            id: columnLayout4
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 0

                            Label {
                                id: label1
                                text: qsTr("TranspersonalPsy")
                                Layout.fillHeight: false
                            }

                            Label {
                                id: label2
                                text: qsTr("MMR 100500")
                                Layout.fillHeight: false
                            }

                            Label {
                                id: label4
                                text: qsTr("Games Played 250000")
                            }

                            Label {
                                id: label5
                                text: qsTr("Win rate 100%")
                            }

                            Label {
                                id: label6
                                text: qsTr("APM 450")
                            }
                        }
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
