import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ColumnLayout
{
    spacing: 0
    property Rectangle expandButtonRectangle : expandButtonRectangle
    property bool showTrainingModeSwitch: true
    property bool enableTrainingModeSwitch: _uiBackend.enableTrainingModeSwitch

    property int relativeMouseX
    property int relativeMouseY

    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Переключатель "No Fog"
        if (enableTrainingModeSwitch &&
                showTrainingModeSwitch &&
                relativeMouseX >= trainingModeSwitch.x + headerRectangle.x &&
                relativeMouseX <= trainingModeSwitch.x + headerRectangle.x + trainingModeSwitch.width &&
                relativeMouseY >= trainingModeSwitch.y + headerRectangle.height &&
                relativeMouseY <= trainingModeSwitch.y + headerRectangle.height + trainingModeSwitch.height)
        {
            trainingModeSwitch.checkedState = !trainingModeSwitch.checkedState;
            _uiBackend.trainingModeState = trainingModeSwitch.checkedState;
        }
    }

    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Переключатель "No Fog"
        if (enableTrainingModeSwitch &&
                showTrainingModeSwitch &&
                relativeMouseX >= trainingModeSwitch.x + headerRectangle.x &&
                relativeMouseX <= trainingModeSwitch.x + headerRectangle.x + trainingModeSwitch.width &&
                relativeMouseY >= trainingModeSwitch.y + headerRectangle.height &&
                relativeMouseY <= trainingModeSwitch.y + headerRectangle.height + trainingModeSwitch.height)
        {
            trainingModeSwitch.hoverState = true;
        } else {
            trainingModeSwitch.hoverState = false;
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



        //function getObj() { return obj }

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

        Layout.fillHeight: false





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
                    //source: "qrc:/images/resources/images/ssstats.png"
                    source: "qrc:/images/resources/images/rank6_60.png"
                    anchors.rightMargin: 10 * _uiBackend.sizeModifer
                    anchors.leftMargin: 10 * _uiBackend.sizeModifer
                    anchors.bottomMargin: 10 * _uiBackend.sizeModifer
                    anchors.topMargin: 10 * _uiBackend.sizeModifer
                    Layout.minimumHeight: 60 * _uiBackend.sizeModifer
                    Layout.minimumWidth: 60 * _uiBackend.sizeModifer
                    Layout.margins: 0 * _uiBackend.sizeModifer
                    Layout.rightMargin: 0 * _uiBackend.sizeModifer
                    Layout.leftMargin: 0 * _uiBackend.sizeModifer
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    fillMode: Image.PreserveAspectFit
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

        color: trainingModeSwitch.checked ? "#ffa9a9" : "#00ff99";
        radius: 10 * _uiBackend.sizeModifer

        visible: showTrainingModeSwitch

        Rectangle{
            width: parent.width
            height: 10 * _uiBackend.sizeModifer

            color: trainingModeSwitch.checked ? "#ffa9a9" : "#00ff99";
        }

        Switch
        {
            id: trainingModeSwitch
            anchors.fill: parent
            text: "Training mode"

            property bool hoverState : false
            property bool checkedState : _uiBackend.trainingModeState

            font.pixelSize: 15 * _uiBackend.sizeModifer
            opacity: hoverState ? 1.0 : 0.8
            checked: checkedState
            leftPadding: 10 * _uiBackend.sizeModifer

        }
    }

}



