import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: settingsRectangle
    opacity: 1
    color: "#f9f9f9"
    radius: 10 * _uiBackend.sizeModifer
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60 * _uiBackend.sizeModifer
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    property int relativeMouseX
    property int relativeMouseY


    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Переключатель "No Fog"
        if (relativeMouseX >= noFogSwitch.x + settingsColumn.x &&
                relativeMouseX <= noFogSwitch.x + settingsColumn.x + noFogSwitch.width &&
                relativeMouseY >= noFogSwitch.y + settingsColumn.y &&
                relativeMouseY <= noFogSwitch.y + settingsColumn.y +noFogSwitch.height)
        {
            noFogSwitch.checkedState = !noFogSwitch.checkedState;
            _uiBackend.noFogState = noFogSwitch.checkedState;
        }

        // Переключатель "Game panel visisble"
        if (relativeMouseX >= showGamePannelPresetSwitch.x + settingsColumn.x &&
                relativeMouseX <= showGamePannelPresetSwitch.x + settingsColumn.x + showGamePannelPresetSwitch.width &&
                relativeMouseY >= showGamePannelPresetSwitch.y + settingsColumn.y &&
                relativeMouseY <= showGamePannelPresetSwitch.y + settingsColumn.y +showGamePannelPresetSwitch.height)
        {
            showGamePannelPresetSwitch.checkedState = !showGamePannelPresetSwitch.checkedState;
            _uiBackend.gamePanel.showGamePannelPreset = showGamePannelPresetSwitch.checkedState;
        }



        // Переключатель "Small panel"
        if (relativeMouseX >= smallGamePanelActivateSwitch.x + settingsColumn.x &&
                relativeMouseX <= smallGamePanelActivateSwitch.x + settingsColumn.x + smallGamePanelActivateSwitch.width &&
                relativeMouseY >= smallGamePanelActivateSwitch.y + settingsColumn.y &&
                relativeMouseY <= smallGamePanelActivateSwitch.y + settingsColumn.y +smallGamePanelActivateSwitch.height)
        {
            smallGamePanelActivateSwitch.checkedState = !smallGamePanelActivateSwitch.checkedState;
            _uiBackend.gamePanel.smallPannelActive = smallGamePanelActivateSwitch.checkedState
        }


        //Scale 50%
        if (relativeMouseX >= scaleRadioButton0.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton0.x + settingsColumn.x + scaleRadioButton0.width &&
                relativeMouseY >= scaleRadioButton0.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton0.y + settingsColumn.y +scaleRadioButton0.height)
        {

            if (!scaleRadioButton0.checked)
                _uiBackend.setSizeModifer(0.5);

            scaleRadioButton0.checked = true;
        }

        //Scale 75%
        if (relativeMouseX >= scaleRadioButton1.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton1.x + settingsColumn.x + scaleRadioButton1.width &&
                relativeMouseY >= scaleRadioButton1.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton1.y + settingsColumn.y +scaleRadioButton1.height)
        {
            if (!scaleRadioButton1.checked)
                _uiBackend.setSizeModifer(0.75);

            scaleRadioButton1.checked = true;
        }


        //Scale 100%
        if (relativeMouseX >= scaleRadioButton2.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton2.x + settingsColumn.x + scaleRadioButton2.width &&
                relativeMouseY >= scaleRadioButton2.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton2.y + settingsColumn.y +scaleRadioButton2.height)
        {
            if (!scaleRadioButton2.checked)
                _uiBackend.setSizeModifer(1.0);

            scaleRadioButton2.checked = true;
        }

        //Scale 125%
        if (relativeMouseX >= scaleRadioButton3.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton3.x + settingsColumn.x + scaleRadioButton3.width &&
                relativeMouseY >= scaleRadioButton3.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton3.y + settingsColumn.y +scaleRadioButton3.height)
        {
            if (!scaleRadioButton3.checked)
                _uiBackend.setSizeModifer(1.25);

            scaleRadioButton3.checked = true;
        }

        //Scale 150%
        if (relativeMouseX >= scaleRadioButton4.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton4.x + settingsColumn.x + scaleRadioButton4.width &&
                relativeMouseY >= scaleRadioButton4.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton4.y + settingsColumn.y +scaleRadioButton4.height)
        {
            if (!scaleRadioButton4.checked)
                _uiBackend.setSizeModifer(1.5);

            scaleRadioButton4.checked = true;
        }

        //Scale 175%
        if (relativeMouseX >= scaleRadioButton5.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton5.x + settingsColumn.x + scaleRadioButton5.width &&
                relativeMouseY >= scaleRadioButton5.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton5.y + settingsColumn.y +scaleRadioButton5.height)
        {
            if (!scaleRadioButton5.checked)
                _uiBackend.setSizeModifer(1.75);

            scaleRadioButton5.checked = true;
        }

        //Scale 200%
        if (relativeMouseX >= scaleRadioButton6.x + settingsColumn.x &&
                relativeMouseX <= scaleRadioButton6.x + settingsColumn.x + scaleRadioButton6.width &&
                relativeMouseY >= scaleRadioButton6.y + settingsColumn.y &&
                relativeMouseY <= scaleRadioButton6.y + settingsColumn.y +scaleRadioButton6.height)
        {

            if (!scaleRadioButton6.checked)
                _uiBackend.setSizeModifer(2.0);

            scaleRadioButton6.checked = true;
        }
    }

    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Переключатель "No Fog"
        if (relativeMouseX >= noFogSwitch.x + settingsColumn.x &&
                relativeMouseX <= noFogSwitch.x + settingsColumn.x + noFogSwitch.width &&
                relativeMouseY >= noFogSwitch.y + settingsColumn.y&&
                relativeMouseY <= noFogSwitch.y + settingsColumn.y + noFogSwitch.height)
        {
            noFogSwitch.hoverState = true;
        } else {
            noFogSwitch.hoverState = false;
        }

        // Переключатель "Game panel visisble"
        if (relativeMouseX >= showGamePannelPresetSwitch.x + settingsColumn.x &&
                relativeMouseX <= showGamePannelPresetSwitch.x + settingsColumn.x + showGamePannelPresetSwitch.width &&
                relativeMouseY >= showGamePannelPresetSwitch.y + settingsColumn.y&&
                relativeMouseY <= showGamePannelPresetSwitch.y + settingsColumn.y + showGamePannelPresetSwitch.height)
        {
            showGamePannelPresetSwitch.hoverState = true;
        } else {
            showGamePannelPresetSwitch.hoverState = false;
        }


        // Переключатель "Small panel"
        if (relativeMouseX >= smallGamePanelActivateSwitch.x + settingsColumn.x &&
                relativeMouseX <= smallGamePanelActivateSwitch.x + settingsColumn.x + smallGamePanelActivateSwitch.width &&
                relativeMouseY >= smallGamePanelActivateSwitch.y + settingsColumn.y&&
                relativeMouseY <= smallGamePanelActivateSwitch.y + settingsColumn.y + smallGamePanelActivateSwitch.height)
        {
            smallGamePanelActivateSwitch.hoverState = true;
        } else {
            smallGamePanelActivateSwitch.hoverState = false;
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


    Text {
        id: settingsLabel
        height: 40 * _uiBackend.sizeModifer
        text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        textFormat: Text.RichText
        anchors.topMargin: 0
        anchors.leftMargin: 0
        anchors.rightMargin: 0
    }

    Column {
        id: settingsColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: settingsLabel.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10 * _uiBackend.sizeModifer
        anchors.leftMargin: 10 * _uiBackend.sizeModifer
        anchors.bottomMargin: 10 * _uiBackend.sizeModifer
        anchors.topMargin: 10 * _uiBackend.sizeModifer

        Switch {
            id: noFogSwitch

            property bool hoverState : false
            property bool checkedState : _uiBackend.noFogState

            text: qsTr("Remove fog (aka No Fog) of the game space when moving away")
            //font.pointSize: 10
            font.pixelSize: 15 * _uiBackend.sizeModifer
            opacity: hoverState ? 1.0 : 0.8
            checked: checkedState
        }

        Switch {
            id: showGamePannelPresetSwitch

            property bool hoverState : false
            property bool checkedState : _uiBackend.gamePanel.showGamePannelPreset

            text: qsTr("APM panel visible in game")
            //font.pointSize: 10
            font.pixelSize: 15 * _uiBackend.sizeModifer
            opacity: hoverState ? 1.0 : 0.8
            checked: checkedState
        }

        Switch {
            id: smallGamePanelActivateSwitch

            property bool hoverState : false
            property bool checkedState : _uiBackend.gamePanel.smallPannelActive

            text: qsTr("Small APM panel in game")
            //font.pointSize: 10
            font.pixelSize: 15 * _uiBackend.sizeModifer

            opacity: hoverState ? 1.0 : 0.8
            checked: checkedState
        }

        Label {
            id: label
            text: qsTr("Scale")
            leftPadding: 10
            font.pointSize: 10
            topPadding: 20
        }

        RadioButton {
            id: scaleRadioButton0
            text: qsTr("50%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton1
            text: qsTr("75%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton2
            text: qsTr("100%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton3
            text: qsTr("125%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton4
            text: qsTr("150%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton5
            text: qsTr("175%")
            leftPadding: 20
        }

        RadioButton {
            id: scaleRadioButton6
            text: qsTr("200%")
            leftPadding: 20
        }

    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
