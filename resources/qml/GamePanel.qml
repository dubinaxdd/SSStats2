import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rectangle
    color: "#00000000"
    anchors.fill: parent
    visible: model.gamePanelVisible && !model.smallPannelActive && model.showGamePannelPreset

    property var model

    function chooseColor(string)
    {
        if (string.contains("Team 1"))
            return "#b3ea0000"
    }

    property int relativeMouseX
    property int relativeMouseY

    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Кнопка "Развернуть оверлей в игровой панели"
        if (relativeMouseX >= expandButtonRectangle.x + gamePanelRectangle.x &&
                relativeMouseX <= expandButtonRectangle.x + gamePanelRectangle.x + expandButtonRectangle.width &&
                relativeMouseY >= expandButtonRectangle.y + gamePanelRectangle.y &&
                relativeMouseY <= expandButtonRectangle.y + gamePanelRectangle.y + expandButtonRectangle.height)
        {

            if(_uiBackend.gamePanel.gamePanelVisible)
            {
                expandButtonRectangle.howeredState = true;
                _uiBackend.expandKeyPressed();
            }
        }

        // Кнопка "Развернуть панель с рассами игроков"
        if (relativeMouseX >= expandPlayerRacesButton.x + gamePanelRectangle.x &&
                relativeMouseX <= expandPlayerRacesButton.x + gamePanelRectangle.x + expandPlayerRacesButton.width &&
                relativeMouseY >= expandPlayerRacesButton.y + gamePanelRectangle.y  + rectangle1.height + 5 &&
                relativeMouseY <= expandPlayerRacesButton.y + gamePanelRectangle.y  + rectangle1.height + 5 + expandPlayerRacesButton.height)
        {

            if(_uiBackend.gamePanel.gamePanelVisible)
            {
                expandPlayerRacesButton.howeredState = true;
                model.expandPlayerRacesButtonClick();
            }
        }

    }

    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Кнопка "Развернуть оверлей в игровой панели"
        if (relativeMouseX >= expandButtonRectangle.x + gamePanelRectangle.x &&
                relativeMouseX <= expandButtonRectangle.x + gamePanelRectangle.x + expandButtonRectangle.width &&
                relativeMouseY >= expandButtonRectangle.y + gamePanelRectangle.y + 5 &&
                relativeMouseY <= expandButtonRectangle.y + gamePanelRectangle.y + 5 + expandButtonRectangle.height)
        {

            if(!expandButtonRectangle.howeredState)
                expandButtonRectangle.howeredState = true;
        }
        else
        {
            if(expandButtonRectangle.howeredState)
                expandButtonRectangle.howeredState = false;
        }


        // Кнопка "Развернуть панель с рассами игроков"
        if (relativeMouseX >= expandPlayerRacesButton.x + gamePanelRectangle.x &&
                relativeMouseX <= expandPlayerRacesButton.x + gamePanelRectangle.x + expandPlayerRacesButton.width &&
                relativeMouseY >= expandPlayerRacesButton.y + gamePanelRectangle.y  + rectangle1.height + 5 &&
                relativeMouseY <= expandPlayerRacesButton.y + gamePanelRectangle.y  + rectangle1.height + 5 + expandPlayerRacesButton.height)
        {

            if(!expandPlayerRacesButton.howeredState)
                expandPlayerRacesButton.howeredState = true;
        }
        else
        {
            if(expandPlayerRacesButton.howeredState)
               expandPlayerRacesButton.howeredState = false;
        }
    }



    Rectangle {
        id: gamePanelRectangle
        width: 550
        height: 200
        color: "#00000000"
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent

            Rectangle {
                id: rectangle1
                width: 200
                height: 200
                //color: "#333333"
                Layout.maximumHeight: 40
                Layout.fillHeight: false
                Layout.fillWidth: true

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


                RowLayout {
                    id: rowLayout
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        id: rectangle3
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.minimumHeight: 40
                        Layout.minimumWidth: 40
                        Layout.maximumHeight: 40
                        Layout.maximumWidth: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true


                        Image {
                            id: image
                            anchors.fill: parent
                            source: "qrc:/images/resources/images/ssstats.png"
                            anchors.rightMargin: 5
                            anchors.leftMargin: 5
                            anchors.bottomMargin: 5
                            anchors.topMargin: 5
                            Layout.maximumHeight: 40
                            Layout.maximumWidth: 40
                            fillMode: Image.PreserveAspectFit
                        }
                    }

                    Label {
                        id: label
                        color: "#ffffff"
                        text: qsTr("Current APM: ")
                        font.pointSize: 12
                    }

                    Label {
                        id: currentApmCountLabel
                        color: "#ffffff"
                        text: model.currentApm
                        font.pointSize: 12
                    }

                    Rectangle {
                        id: rectangle5
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.minimumHeight: 40
                        Layout.minimumWidth: 20
                        Layout.maximumHeight: 40
                        Layout.maximumWidth: 20
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        id: label2
                        color: "#ffffff"
                        text: qsTr("Average APM: ")
                        font.pointSize: 12
                    }

                    Label {
                        id: averageApmCountLabel
                        color: "#ffffff"
                        text: model.averageApm
                        font.pointSize: 12
                    }

                    Rectangle {
                        id: rectangle4
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        id: label4
                        visible: model.gameLeaveTimerVisible
                        color: "#d62525"
                        text: qsTr("You can leave: ")
                        font.pointSize: 12
                    }

                    Label {
                        id: timeLeftLabel
                        color: "#d62525"
                        visible: model.gameLeaveTimerVisible
                        text: model.gameLeaveTimeLeft != 0 ? model.gameLeaveTimeLeft : "Is over"
                        font.pointSize: 12
                    }

                    Rectangle {
                        id: rectangle6
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.minimumWidth: 20
                        Layout.minimumHeight: 40
                        Layout.maximumHeight: 40
                        Layout.fillWidth: true
                        Layout.maximumWidth: 20
                        Layout.fillHeight: true
                    }

                    Rectangle {
                        id: expandButtonRectangle

                        property bool howeredState: false

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


                        width: 200
                        height: 200
                        color: "#ffffff"
                        Layout.topMargin: 0
                        Layout.maximumHeight: 40
                        Layout.maximumWidth: 40
                        Layout.fillWidth: false
                        Layout.fillHeight: true

                        gradient: howeredState ? grDark : grLight

                        BorderImage {
                            id: borderImage
                            anchors.fill: parent
                            source: _uiBackend.expand ? "qrc:/images/resources/images/hide.png" : "qrc:/images/resources/images/expand.png"
                        }
                    }
                }
            }

            Rectangle {
                id: playerRacesRectangle
                width: 200
                height: 200
                Layout.fillHeight: false
                Layout.fillWidth: true
                color: "#00000000"
                //visible: model.racePanelVisible

                ColumnLayout {
                    id: columnLayout1
                    anchors.fill: parent

                    Rectangle {
                        id: rectangle8
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.maximumHeight: 150
                        Layout.maximumWidth: 65535
                        Layout.fillHeight: false
                        Layout.fillWidth: true

                        ColumnLayout {
                            id: columnLayout2
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                id: player0Rectangle
                                width: 200
                                height: 20
                                opacity: 1
                                color: model.player0Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player0Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player0RaceLabel
                                    height: 20
                                    text: model.player0Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: model.player0Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player1Rectangle
                                width: 200
                                height: 20
                                color: model.player1Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player1Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player1RaceLabel
                                    height: 20
                                    text: model.player1Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: model.player1Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player2Rectangle
                                width: 200
                                height: 20
                                color: model.player2Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player2Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player2RaceLabel
                                    text: model.player2Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player2Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player3Rectangle
                                width: 200
                                height: 20
                                color: model.player3Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player3Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player3RaceLabel
                                    text: model.player3Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player3Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player4Rectangle
                                width: 200
                                height: 200
                                color: model.player4Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player4Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player4RaceLabel
                                    text: model.player4Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player4Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player5Rectangle
                                width: 200
                                height: 200
                                color: model.player5Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player5Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player5RaceLabel
                                    text: model.player5Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player5Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player6Rectangle
                                width: 200
                                height: 200
                                color: model.player6Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player6Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player6RaceLabel
                                    text: model.player6Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player6Race !== "";
                                    color: "#ffffff"
                                }
                            }


                            Rectangle {
                                id: player7Rectangle
                                width: 200
                                height: 200
                                color: model.player7Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player7Race !== "" && model.racePanelVisible;

                                Label {
                                    id: player7RaceLabel
                                    text: model.player7Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    Layout.fillHeight: false
                                    visible: model.player7Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: rectangle9
                                width: 200
                                height: 200
                                color: "#00000000"
                                border.color: "#00000000"
                                Layout.maximumHeight: 3
                                Layout.minimumHeight: 3
                                Layout.maximumWidth: 65526
                                Layout.minimumWidth: 0
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                visible: model.racePanelVisible;
                            }

                            Rectangle {
                                id: expandPlayerRacesButton

                                property bool howeredState: false

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
                                height: 10
                                radius: 5
                                Layout.maximumHeight: 10
                                Layout.minimumHeight: 10
                                Layout.maximumWidth: 65535
                                Layout.minimumWidth: 0
                                //text: qsTr("***")
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                visible: !_uiBackend.expand
                                color: "#000000"


                                gradient: howeredState ? grDark : grLight

                                Image {
                                    id: imagebutton
                                    width: 95
                                    height: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    source: "qrc:/images/resources/images/expandDots.png"
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    fillMode: Image.PreserveAspectFit
                                }
                            }

                            Rectangle {
                                id: rectangle2
                                width: 200
                                height: 200
                                color: "#00000000"
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }

                        }
                    }

                    Rectangle {
                        id: rectangle7
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }
            }

            Rectangle {
                id: rectangle10
                width: 200
                height: 200
                opacity: 1
                color: "#00000000"
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }

}



/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
