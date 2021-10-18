import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rectangle
    color: "#00000000"
    anchors.fill: parent
    visible: model.gamePanelVisible && model.smallPannelActive && model.showGamePannelPreset

    property var model

    property int relativeMouseX
    property int relativeMouseY

    function chooseColor(string)
    {
        if (string.contains("Team 1"))
            return "#b3ea0000"
    }


    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Кнопка "Развернуть панель с рассами игроков"
        if (relativeMouseX >= expandPlayerRacesButton.x  + gamePanelRectangle.x &&
                relativeMouseX <= expandPlayerRacesButton.x  + gamePanelRectangle.x + expandPlayerRacesButton.width &&
                relativeMouseY >= expandPlayerRacesButton.y + gamePanelRectangle.y &&
                relativeMouseY <= expandPlayerRacesButton.y + gamePanelRectangle.y + expandPlayerRacesButton.height)
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

        // Кнопка "Развернуть панель с рассами игроков"
        if (relativeMouseX >= expandPlayerRacesButton.x  + gamePanelRectangle.x &&
                relativeMouseX <= expandPlayerRacesButton.x  + gamePanelRectangle.x + expandPlayerRacesButton.width &&
                relativeMouseY >= expandPlayerRacesButton.y + gamePanelRectangle.y &&
                relativeMouseY <= expandPlayerRacesButton.y + gamePanelRectangle.y + expandPlayerRacesButton.height)
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
        width: 200
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
                    id: rowLayout1
                    anchors.fill: parent

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

                    GridLayout {
                        id: gridLayout
                        width: 100
                        height: 100
                        Layout.rowSpan: 0
                        rows: 3
                        columns: 2

                        Label {
                            id: label
                            visible: !model.gameLeaveTimerVisible
                            color: "#ffffff"
                            text: qsTr("Current APM: ")
                            font.pointSize: 8
                        }

                        Label {
                            id: currentApmCountLabel
                            visible: !model.gameLeaveTimerVisible
                            color: "#ffffff"
                            text: model.currentApm
                            font.pointSize: 8
                        }

                        Label {
                            id: label2
                            visible: !model.gameLeaveTimerVisible
                            color: "#ffffff"
                            text: qsTr("Average APM: ")
                            font.pointSize: 8
                        }

                        Label {
                            id: averageApmCountLabel
                            visible: !model.gameLeaveTimerVisible
                            color: "#ffffff"
                            text: model.averageApm
                            font.pointSize: 8
                        }

                        Label {
                            id: label4
                            visible: model.gameLeaveTimerVisible
                            color: "#d62525"
                            text: qsTr("You can leave: ")
                            font.pointSize: 10
                        }

                        Label {
                            id: timeLeftLabel
                            color: "#d62525"
                            visible: model.gameLeaveTimerVisible
                            text: model.gameLeaveTimeLeft != 0 ? model.gameLeaveTimeLeft : "Is over"
                            font.pointSize: 10
                        }
                    }

                    Rectangle {
                        id: rectangle4
                        width: 200
                        height: 200
                        color: "#00000000"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
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
                        radius: 0
                        border.width: 0
                        Layout.maximumHeight: 40
                        Layout.minimumHeight: 40
                        Layout.maximumWidth: 40
                        Layout.minimumWidth: 40
                        //text: qsTr("***")
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        visible: !_uiBackend.expand
                        color: "#000000"


                        gradient: howeredState ? grDark : grLight

                        Image {
                            id: imagebutton
                            anchors.fill: parent
                            source: "qrc:/images/resources/images/expandDots.png"
                            fillMode: Image.PreserveAspectFit
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
    D{i:0;autoSize:true;height:480;width:640}D{i:25}D{i:7}
}
##^##*/
