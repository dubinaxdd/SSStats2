import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: rectangle
    color: "#00000000"
    anchors.fill: parent
    visible: _uiBackend.gamePanelVisible


    function chooseColor(string)
    {
        if (string.contains("Team 1"))
            return "#b3ea0000"
    }

    Rectangle {
        id: rectangle9
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
                color: "#333333"
                Layout.maximumHeight: 40
                Layout.fillHeight: false
                Layout.fillWidth: true


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
                        id: label1
                        color: "#ffffff"
                        text: qsTr("450")
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
                        id: label3
                        color: "#ffffff"
                        text: qsTr("450")
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
                        color: "#d62525"
                        text: qsTr("You can leave: ")
                        font.pointSize: 12
                    }

                    Label {
                        id: label5
                        color: "#d62525"
                        text: qsTr("1:00")
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
                        width: 200
                        height: 200
                        color: "#ffffff"
                        Layout.maximumHeight: 40
                        Layout.maximumWidth: 40
                        Layout.fillWidth: false
                        Layout.fillHeight: true


                        gradient: Gradient {
                            GradientStop {
                                position: 0
                                color: "#337ab7"
                            }

                            GradientStop {
                                position: 1
                                color: "#265a88"
                            }
                        }

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
                visible: _uiBackend.racePanelVisible
                color: "#00000000"

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
                                color: _uiBackend.player0Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: _uiBackend.player0Race !== "";

                                Label {
                                    id: player0RaceLabel
                                    height: 20
                                    text: _uiBackend.player0Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player0Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player1Rectangle
                                width: 200
                                height: 20
                                color: _uiBackend.player1Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: _uiBackend.player1Race !== "";

                                Label {
                                    id: player1RaceLabel
                                    height: 20
                                    text: _uiBackend.player1Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillHeight: false
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player1Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player2Rectangle
                                width: 200
                                height: 20
                                color: _uiBackend.player2Color
                                Layout.maximumHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                                visible: _uiBackend.player2Race !== "";

                                Label {
                                    id: player2RaceLabel
                                    text: _uiBackend.player2Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player2Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player3Rectangle
                                width: 200
                                height: 20
                                color: _uiBackend.player3Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: _uiBackend.player3Race !== "";

                                Label {
                                    id: player3RaceLabel
                                    text: _uiBackend.player3Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player3Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player4Rectangle
                                width: 200
                                height: 200
                                color: _uiBackend.player4Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: _uiBackend.player4Race !== "";

                                Label {
                                    id: player4RaceLabel
                                    text: _uiBackend.player4Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player4Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player5Rectangle
                                width: 200
                                height: 200
                                color: _uiBackend.player5Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: _uiBackend.player5Race !== "";

                                Label {
                                    id: player5RaceLabel
                                    text: _uiBackend.player5Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player5Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player6Rectangle
                                width: 200
                                height: 200
                                color: _uiBackend.player6Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: _uiBackend.player6Race !== "";

                                Label {
                                    id: player6RaceLabel
                                    text: _uiBackend.player6Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    visible: _uiBackend.player6Race !== "";
                                    color: "#ffffff"
                                }
                            }

                            Rectangle {
                                id: player7Rectangle
                                width: 200
                                height: 200
                                color: _uiBackend.player7Color
                                Layout.maximumHeight: 20
                                Layout.fillHeight: false
                                Layout.fillWidth: true
                                visible: _uiBackend.player7Race !== "";

                                Label {
                                    id: player7RaceLabel
                                    text: _uiBackend.player7Race
                                    anchors.fill: parent
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.fillWidth: true
                                    Layout.fillHeight: false
                                    visible: _uiBackend.player7Race !== "";
                                    color: "#ffffff"
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
