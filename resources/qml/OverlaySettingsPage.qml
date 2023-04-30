import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: settingsRectangle
    opacity: 1
    color: DowStatsStyle.backgroundColor
    radius: 10 * _uiBackend.sizeModifer
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60 * _uiBackend.sizeModifer
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    Connections
    {
        target: _uiBackend

        function onNoFogStateChanged()
        {
            noFogSwitch.checkedState = _uiBackend.noFogState;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onShowGamePannelPresetChanged()
        {
            showGamePannelPresetSwitch.checkedState =  _uiBackend.gamePanel.showGamePannelPreset;
        }
    }

    Connections
    {
        target: _uiBackend.gamePanel

        function onSmallPannelActiveChanged()
        {
            smallGamePanelActivateSwitch.checkedState = _uiBackend.gamePanel.smallPannelActive;
        }
    }

    Connections{
        target: _uiBackend


        function onSizeModiferLoadedFromSettings(scale)
        {
            switch (scale)
            {
                case 0.5: scaleRadioButton0.checked = true; break;
                case 0.75: scaleRadioButton1.checked = true; break;
                case 1.0: scaleRadioButton2.checked = true; break;
                case 1.25: scaleRadioButton3.checked = true; break;
                case 1.5: scaleRadioButton4.checked = true; break;
                case 1.75: scaleRadioButton5.checked = true; break;
                case 2.0: scaleRadioButton6.checked = true; break;
            }
        }
    }

    ColumnLayout {
        id: settingsColumn

        anchors.fill: parent

        anchors.rightMargin: 10 * _uiBackend.sizeModifer
        anchors.leftMargin: 10 * _uiBackend.sizeModifer
        anchors.bottomMargin: 10 * _uiBackend.sizeModifer
        anchors.topMargin: 10 * _uiBackend.sizeModifer

        StyledSwitch {
            id: noFogSwitch

            property bool hoverState : noFogMouseArea.hovered
            property bool checkedState : _uiBackend.noFogState

            text: qsTr("No FOG")
            fontPxSize: 15 * _uiBackend.sizeModifer
            checked: checkedState
            leftPadding: 10 * _uiBackend.sizeModifer
            sizeModifer: _uiBackend.sizeModifer
            hoveredState: noFogMouseArea.hovered

            GlobalMouseArea{
                id: noFogMouseArea
                anchors.fill: parent

                onClicked: {
                    noFogSwitch.checkedState = !noFogSwitch.checkedState;
                    _uiBackend.noFogState = noFogSwitch.checkedState;
                }
            }
        }

        StyledSwitch {
            id: showGamePannelPresetSwitch

            property bool hoverState : showGamePannelMouseArea.hovered
            property bool checkedState : _uiBackend.gamePanel.showGamePannelPreset

            text: qsTr("APM panel visible in game")
            fontPxSize: 15 * _uiBackend.sizeModifer
            checked: checkedState
            leftPadding: 10 * _uiBackend.sizeModifer
            sizeModifer: _uiBackend.sizeModifer
            hoveredState: showGamePannelMouseArea.hovered

            GlobalMouseArea{
                id: showGamePannelMouseArea
                anchors.fill: parent

                onClicked: {
                    showGamePannelPresetSwitch.checkedState = !showGamePannelPresetSwitch.checkedState;
                    _uiBackend.gamePanel.showGamePannelPreset = showGamePannelPresetSwitch.checkedState;
                }
            }
        }

        StyledSwitch {
            id: smallGamePanelActivateSwitch

            property bool hoverState : smallGamePanelActivateMouseArea.hovered
            property bool checkedState : _uiBackend.gamePanel.smallPannelActive

            text: qsTr("Small APM panel in game")
            fontPxSize: 15 * _uiBackend.sizeModifer
            checked: checkedState
            sizeModifer: _uiBackend.sizeModifer
            leftPadding: 10 * _uiBackend.sizeModifer
            hoveredState: smallGamePanelActivateMouseArea.hovered

            GlobalMouseArea{
                id: smallGamePanelActivateMouseArea
                anchors.fill: parent

                onClicked: {
                    smallGamePanelActivateSwitch.checkedState = !smallGamePanelActivateSwitch.checkedState;
                    _uiBackend.gamePanel.smallPannelActive = smallGamePanelActivateSwitch.checkedState
                }
            }
        }

        Label {
            id: label
            text: qsTr("Scale")
            leftPadding: 10 * _uiBackend.sizeModifer
            font.pixelSize: 15 * _uiBackend.sizeModifer
            topPadding: 20 * _uiBackend.sizeModifer
            color: DowStatsStyle.textColor
        }

        StyledRadioButton {
            id: scaleRadioButton0

            property bool hoverState: scaleRadioButton0MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("50%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton0MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton0.checked)
                        _uiBackend.setSizeModifer(0.5);

                    scaleRadioButton0.checked = true;
                }
            }

        }

        StyledRadioButton {
            id: scaleRadioButton1

            property bool hoverState : scaleRadioButton1MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("75%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer


            GlobalMouseArea{
                id: scaleRadioButton1MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton1.checked)
                        _uiBackend.setSizeModifer(0.75);

                    scaleRadioButton1.checked = true;
                }
            }
        }

        StyledRadioButton {
            id: scaleRadioButton2

            property bool hoverState : scaleRadioButton2MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("100%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton2MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton2.checked)
                        _uiBackend.setSizeModifer(1.0);

                    scaleRadioButton2.checked = true;
                }
            }
        }

        StyledRadioButton {
            id: scaleRadioButton3

            property bool hoverState : scaleRadioButton3MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("125%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton3MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton3.checked)
                        _uiBackend.setSizeModifer(1.25);

                    scaleRadioButton3.checked = true;
                }
            }

        }

        StyledRadioButton {
            id: scaleRadioButton4

            property bool hoverState : scaleRadioButton4MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("150%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton4MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton4.checked)
                        _uiBackend.setSizeModifer(1.5);

                    scaleRadioButton4.checked = true;
                }
            }
        }

        StyledRadioButton {
            id: scaleRadioButton5

            property bool hoverState : scaleRadioButton5MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("175%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton5MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton5.checked)
                        _uiBackend.setSizeModifer(1.75);

                    scaleRadioButton5.checked = true;
                }
            }
        }

        StyledRadioButton {
            id: scaleRadioButton6

            property bool hoverState : scaleRadioButton6MouseArea.hovered
            opacity: hoverState ? 1.0 : 0.8

            text: qsTr("200%")
            leftPadding: 20
            fontPxSize: 15 * _uiBackend.sizeModifer

            GlobalMouseArea{
                id: scaleRadioButton6MouseArea
                anchors.fill: parent

                onClicked: {
                    if (!scaleRadioButton6.checked)
                        _uiBackend.setSizeModifer(2.0);

                    scaleRadioButton6.checked = true;
                }
            }
        }

        Rectangle{
            Layout.fillHeight: true
        }

    }
}
