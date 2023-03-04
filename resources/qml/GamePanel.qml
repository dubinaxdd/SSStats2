import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rectangle
    color: "#00000000"
    anchors.fill: parent


    property var model

    function chooseColor(string)
    {
        if (string.contains("Team 1"))
            return "#b3ea0000"
    }

    Rectangle {
        id: gamePanelRectangle
        width: 550 * _uiBackend.sizeModifer
        height: 200 * _uiBackend.sizeModifer
        color: "#00000000"
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent

            Rectangle {
                id: rectangle1

                visible: model.gamePanelVisible && !model.smallPannelActive && model.showGamePannelPreset

                width: 200 * _uiBackend.sizeModifer
                height: 200 * _uiBackend.sizeModifer
                //color: "#333333"
                Layout.maximumHeight: 40 * _uiBackend.sizeModifer
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
                        id: expandButtonRectangle

                        visible: false

                        property bool howeredState: expandButtonMouseArea.howered

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

                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#ffffff"
                        Layout.topMargin: 0
                        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                        Layout.maximumWidth: 40 * _uiBackend.sizeModifer
                        Layout.fillWidth: false
                        Layout.fillHeight: true

                        gradient: howeredState ? grDark : grLight

                        BorderImage {
                            id: borderImage
                            anchors.fill: parent
                            source: _uiBackend.expand ? "qrc:/images/resources/images/hide.png" : "qrc:/images/resources/images/expand.png"
                        }


                        GlobalMouseArea{
                            id: expandButtonMouseArea
                            anchors.fill: parent

                            onClicked: {
                                _uiBackend.expandKeyPressed();
                            }
                        }
                    }


                    Rectangle {
                        id: rectangle3
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.minimumHeight: 40 * _uiBackend.sizeModifer
                        Layout.minimumWidth: 40 * _uiBackend.sizeModifer
                        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                        Layout.maximumWidth: 40 * _uiBackend.sizeModifer
                        Layout.fillWidth: true
                        Layout.fillHeight: true


                        Image {
                            id: image
                            anchors.fill: parent
                            //source: "qrc:/images/resources/images/ssstats.png"
                            source: "qrc:/images/resources/images/rank6_30.png"
                            anchors.rightMargin: 5 * _uiBackend.sizeModifer
                            anchors.leftMargin: 5 * _uiBackend.sizeModifer
                            anchors.bottomMargin: 5 * _uiBackend.sizeModifer
                            anchors.topMargin: 5 * _uiBackend.sizeModifer
                            Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                            Layout.maximumWidth: 40 * _uiBackend.sizeModifer
                            fillMode: Image.PreserveAspectFit
                        }
                    }

                    Label {
                        id: label
                        color: "#ffffff"
                        text: qsTr("Current APM: ")
                        //font.pointSize: 12

                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Label {
                        id: currentApmCountLabel
                        color: "#ffffff"
                        text: model.currentApm;
                        //font.pointSize: 12
                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Rectangle {
                        id: rectangle5
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.minimumHeight: 40 * _uiBackend.sizeModifer
                        Layout.minimumWidth: 20 * _uiBackend.sizeModifer
                        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                        Layout.maximumWidth: 20 * _uiBackend.sizeModifer
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        id: label2
                        color: "#ffffff"
                        text: qsTr("Average APM: ")
                        //font.pointSize: 12
                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Label {
                        id: averageApmCountLabel
                        color: "#ffffff"
                        text: model.averageApm;
                        //font.pointSize: 12
                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Rectangle {
                        id: rectangle4
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        id: label4
                        visible: model.gameLeaveTimerVisible
                        color: "#d62525"
                        text: qsTr("You can leave: ")
                        //font.pointSize: 12
                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Label {
                        id: timeLeftLabel
                        color: "#d62525"
                        visible: model.gameLeaveTimerVisible
                        text: model.gameLeaveTimeLeft != 0 ? model.gameLeaveTimeLeft : "Is over"
                        //font.pointSize: 12
                        font.pixelSize: 15 * _uiBackend.sizeModifer
                    }

                    Rectangle {
                        id: rectangle6
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.minimumWidth: 20 * _uiBackend.sizeModifer
                        Layout.minimumHeight: 40 * _uiBackend.sizeModifer
                        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                        Layout.fillWidth: true
                        Layout.maximumWidth: 20 * _uiBackend.sizeModifer
                        Layout.fillHeight: true
                    }

                    Rectangle {
                        id: expandPlayerRacesButton

                        property bool howeredState: expandPlayerRacesMouseArea.howered

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
                        height: 10 * _uiBackend.sizeModifer
                        radius: 0
                        border.width: 0
                        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
                        Layout.minimumHeight: 40 * _uiBackend.sizeModifer
                        Layout.maximumWidth: 40 * _uiBackend.sizeModifer
                        Layout.minimumWidth: 40 * _uiBackend.sizeModifer
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

                        GlobalMouseArea{
                            id: expandPlayerRacesMouseArea
                            anchors.fill: parent

                            onClicked: {
                                model.expandPlayerRacesButtonClick();
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: playerRacesRectangle
                width: 200 * _uiBackend.sizeModifer
                height: 200 * _uiBackend.sizeModifer
                Layout.fillHeight: false
                Layout.fillWidth: true
                color: "#00000000"
                //visible: model.racePanelVisible

                ColumnLayout {
                    id: columnLayout1
                    anchors.fill: parent

                    Rectangle {
                        id: rankedModeRectangle
                        width: 200 * _uiBackend.sizeModifer
                        height: 20 * _uiBackend.sizeModifer
                        opacity: 1
                        color: _uiBackend.gamePanel.gameRankedMode ? "#00ff99" : "#ffa9a9";
                        Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        visible: model.racePanelVisible && !model.smallPannelActive && _uiBackend.gamePanel.rankedModePanelVisible
                        radius: 5 * _uiBackend.sizeModifer

                        Label {
                            id: rankedModeLabel
                            height: 20 * _uiBackend.sizeModifer
                            text: _uiBackend.gamePanel.gameRankedMode ? "Ranked mode" : "Unranked mode"
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: "#000000"

                            font.pixelSize: 12 * _uiBackend.sizeModifer
                        }
                    }

                    Rectangle {
                        id: rectangle8
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.maximumHeight: 150 * _uiBackend.sizeModifer
                        Layout.maximumWidth: 65535
                        Layout.fillHeight: false
                        Layout.fillWidth: true

                        ColumnLayout {
                            id: columnLayout2
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                id: player0Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 20 * _uiBackend.sizeModifer
                                opacity: 1
                                color: model.player0Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player0Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player0RaceLabel
                                    height: 20 * _uiBackend.sizeModifer
                                    text: model.player0Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: model.player0Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player1Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 20
                                color: model.player1Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player1Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player1RaceLabel
                                    height: 20 * _uiBackend.sizeModifer
                                    text: model.player1Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: model.player1Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player2Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 20 * _uiBackend.sizeModifer
                                color: model.player2Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: model.player2Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player2RaceLabel
                                    text: model.player2Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player2Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player3Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 20 * _uiBackend.sizeModifer
                                color: model.player3Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player3Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player3RaceLabel
                                    text: model.player3Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player3Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player4Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: model.player4Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player4Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player4RaceLabel
                                    text: model.player4Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player4Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player5Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: model.player5Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player5Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player5RaceLabel
                                    text: model.player5Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player5Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: player6Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: model.player6Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player6Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player6RaceLabel
                                    text: model.player6Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: model.player6Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }


                            Rectangle {
                                id: player7Rectangle
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: model.player7Color
                                Layout.maximumHeight: 20 * _uiBackend.sizeModifer
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: model.player7Race !== "" && model.racePanelVisible && !model.smallPannelActive

                                Label {
                                    id: player7RaceLabel
                                    text: model.player7Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    Layout.fillHeight: false
                                    visible: model.player7Race !== "";
                                    color: "#ffffff"

                                    font.pixelSize: 12 * _uiBackend.sizeModifer
                                }
                            }

                            Rectangle {
                                id: rectangle9
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: "#00000000"
                                border.color: "#00000000"
                                Layout.maximumHeight: 3 * _uiBackend.sizeModifer
                                Layout.minimumHeight: 3 * _uiBackend.sizeModifer
                                Layout.maximumWidth: 65526
                                Layout.minimumWidth: 0
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                visible: model.racePanelVisible;
                            }

                            Rectangle {
                                id: rectangle2
                                width: 200 * _uiBackend.sizeModifer
                                height: 200 * _uiBackend.sizeModifer
                                color: "#00000000"
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }

                        }
                    }

                    Rectangle {
                        id: rectangle7
                        width: 200 * _uiBackend.sizeModifer
                        height: 200 * _uiBackend.sizeModifer
                        color: "#00000000"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }
            }

            Rectangle {
                id: rectangle10
                width: 200 * _uiBackend.sizeModifer
                height: 200 * _uiBackend.sizeModifer
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
