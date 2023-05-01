import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout
{
    id: root
    spacing: 0

    property bool showTrainingModeSwitch: true
    property bool enableTrainingModeSwitch: _uiBackend.enableTrainingModeSwitch

    Connections
    {
        target: _uiBackend

        function onRankedModeStateChanged()
        {
            trainingModeSwitch.checked = _uiBackend.rankedModeState;
        }
    }

    Rectangle {
        id: headerRectangle
        width: 280 * _uiBackend.sizeModifer
        height: 60 * _uiBackend.sizeModifer
        color: "#333333"
        Layout.minimumWidth: 280 * _uiBackend.sizeModifer
        Layout.minimumHeight: 60  * _uiBackend.sizeModifer

        Layout.maximumHeight: 60 * _uiBackend.sizeModifer
        Layout.maximumWidth: 280 * _uiBackend.sizeModifer

        Layout.fillHeight: false

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#333333"
            }

            GradientStop {
                position: 1
                color: "#000000"
                //color: "#48cc11"
            }
        }

        RowLayout {
            id: rowLayout2

            spacing: 0
            anchors.fill: parent

            Rectangle {
                id: rectangle5
                width: 200 * _uiBackend.sizeModifer
                height: 200 * _uiBackend.sizeModifer
                color: "#00000000"
                border.color: "#00000000"
                Layout.maximumHeight: 60 * _uiBackend.sizeModifer
                Layout.maximumWidth: 60 * _uiBackend.sizeModifer
                Layout.minimumHeight: 60 * _uiBackend.sizeModifer
                Layout.minimumWidth: 60 * _uiBackend.sizeModifer
                Layout.fillWidth: true
                Layout.fillHeight: true

                Image {
                    id: image
                    anchors.fill: parent

                    source: "qrc:/images/resources/images/Rank8.svg"
                    anchors.rightMargin: 10 * _uiBackend.sizeModifer
                    anchors.leftMargin: 10 * _uiBackend.sizeModifer
                    anchors.bottomMargin: 10 * _uiBackend.sizeModifer
                    anchors.topMargin: 10 * _uiBackend.sizeModifer

                    fillMode: Image.PreserveAspectFit

                    sourceSize.width: 60 * _uiBackend.sizeModifer
                    sourceSize.height: 60 * _uiBackend.sizeModifer
                }
            }

            ColumnLayout {
                id: columnLayout2
                width: 100 * _uiBackend.sizeModifer
                height: 100 * _uiBackend.sizeModifer
                Layout.fillWidth: false
                Layout.fillHeight: false

                Label {
                    id: label
                    color: "#ffffff"

                    text: qsTr("DoW Stats 2")
                    font.pixelSize: 25 * _uiBackend.sizeModifer
                }

                Label {
                    id: versionLabel
                    color: "#999999"
                    text: "v" + _uiBackend.ssStatsVersion
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    //font.pointSize: 8 * 1.5
                    font.pixelSize: 12 * _uiBackend.sizeModifer
                }
            }

            Rectangle {
                Layout.fillWidth: true
            }

            Rectangle {
                id: expandButtonRectangle

                property bool howeredState: expandButtonMouseArea.hovered

                GlobalMouseArea
                {
                    id: expandButtonMouseArea
                    anchors.fill: parent
                    onClicked: _uiBackend.expandKeyPressed();
                }

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

                //width: 200 * _uiBackend.sizeModifer
                //height: 200 * _uiBackend.sizeModifer
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.preferredHeight: 60 * _uiBackend.sizeModifer
                Layout.preferredWidth: 60 * _uiBackend.sizeModifer
                Layout.columnSpan: 0
                Layout.rowSpan: 0
                Layout.maximumHeight: 60 * _uiBackend.sizeModifer
                Layout.maximumWidth: 60 * _uiBackend.sizeModifer
                Layout.minimumHeight: 60 * _uiBackend.sizeModifer
                Layout.minimumWidth: 60 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true

                gradient: howeredState ? grDark : grLight

                BorderImage {
                    id: borderImage
                    anchors.fill: parent
                    source: _uiBackend.expand ? "qrc:/images/resources/images/hide.png" : "qrc:/images/resources/images/expand.png"
                }
            }
        }
    }

    Rectangle
    {
        Layout.maximumWidth: 280 * _uiBackend.sizeModifer
        Layout.maximumHeight: 40 * _uiBackend.sizeModifer
        Layout.minimumWidth: 280 * _uiBackend.sizeModifer
        Layout.minimumHeight: 40 * _uiBackend.sizeModifer
        height: 40 * _uiBackend.sizeModifer
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        Layout.fillHeight: false
        Layout.fillWidth: true
       // color: "#ffffff"

        color: trainingModeSwitch.checked ? "#00ff99" : "#ffa9a9";
        radius: 10 * _uiBackend.sizeModifer

        visible: showTrainingModeSwitch

        Rectangle{
            width: parent.width
            height: 10 * _uiBackend.sizeModifer

            color: trainingModeSwitch.checked ? "#00ff99" : "#ffa9a9";
        }

        StyledSwitch
        {
            id: trainingModeSwitch
            anchors.fill: parent
            anchors.leftMargin: 10 * _uiBackend.sizeModifer

            text: "Ranked mode"

            property bool hoverState : false
            property bool checkedState : _uiBackend.rankedModeState

            fontPxSize: 15 * _uiBackend.sizeModifer
            checked: checkedState
            leftPadding: 10 * _uiBackend.sizeModifer
            hoveredState: trainingModeSwitchMouseArea.hovered
            sizeModifer: _uiBackend.sizeModifer
            textColor: "#26282a"
            backgroundCheckedColor: "#A9A9A9"
            backgroundUncheckedColor: "#c8c8c8"
            indicatorColor: "#FFFFFF"

            GlobalMouseArea{
                id: trainingModeSwitchMouseArea
                anchors.fill: parent

                onClicked: {
                    trainingModeSwitch.checkedState = !trainingModeSwitch.checkedState;
                    _uiBackend.rankedModeState = trainingModeSwitch.checkedState;
                }
            }
        }
    }
}



