import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import DowStatsStyle 1.0

Rectangle {
    width: 50
    color: "#ffffff"

    gradient: Gradient {
        GradientStop {
            position: 0
            color: DowStatsStyle.backgroundColor
        }

        GradientStop {
            position: 1
            color: DowStatsStyle.itemColor//"#C0C0C0"
        }
    }

    RowLayout
    {
        anchors.fill: parent

        spacing: 15

        Rectangle{
            id: dowstatsButtonRectangle
            width: dowstatsLinkLabel.width + 30
            height: 35
            radius:5
            Layout.leftMargin: 15

            color: "#D2691E"
            /*
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#F4A460"
                }

                GradientStop {
                    position: 1
                    color: "#D2691E"
                }
            }*/

            Label{
                id: dowstatsLinkLabel
                text: "dowstats.ru"
                color: dowstatsLinkMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"

                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            MouseArea{
                id: dowstatsLinkMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    Qt.openUrlExternally("https://dowstats.ru/");
                }
            }
        }

        Rectangle{
            id: discordButtonRectangle
            width: discordLinkLabel.width + 30
            height: 35
            radius:5

            color: "#4169E1"

            Label{
                id: discordLinkLabel
                text: "Discord"
                color: discordLinkMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter

            }

            MouseArea{
                id: discordLinkMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    //Qt.openUrlExternally("https://discordapp.com/channels/606832876369215491/852182655570411592");
                    Qt.openUrlExternally("https://discord.gg/t7WqcDDeSU");

                }
            }
        }


        Rectangle{
            id: dowGuideLinkRectangle
            width: dowGuideLinkLabel.width + 30
            height: 35
            radius:5

            color: "#D2691E"

            Label{
                id: dowGuideLinkLabel
                text: "DoW Player's Guide"
                color: dowGuideLinkMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter

            }

            MouseArea{
                id: dowGuideLinkMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    Qt.openUrlExternally("https://warhammer-guide.ru/wiki/Dawn_of_War_Players_Guide.html");
                }
            }
        }


        Rectangle{
            id: dowOnlineLinkRectangle
            width: dowOnlineRowLayout.width + 30
            height: 35
            radius:5
            color: "#D2691E"

            RowLayout
            {
                id: dowOnlineRowLayout

                Image{
                    source: "qrc:/images/resources/images/dow_online.png"
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    Layout.leftMargin: 15
                    Layout.topMargin: 2.5
                }

                Label{
                    id: dowOnlineLinkLabel
                    text: "DowOnline"
                    color: dowOnlineLinkMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                    font.pixelSize: 16
                }
            }

            MouseArea{
                id: dowOnlineLinkMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    Qt.openUrlExternally("https://dowonline.ru");
                }
            }
        }

        Rectangle{
            id: helpLinkRectangle
            //width: helpGuideLinkLabel.width + 30
            Layout.preferredWidth: helpGuideLinkLabel.width + 30
            height: 35
            radius:5

            color: "#D2691E"

            Label{
                id: helpGuideLinkLabel
                text: qsTr("Help")
                color: helpGuideLinkMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter

            }

            MouseArea{
                id: helpGuideLinkMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    Qt.openUrlExternally("https://dowstats.ru/support.php");
                }
            }
        }


        Rectangle{
            Layout.fillWidth: true
        }



       /* ColumnLayout
        {
            clip: true

            visible: _uiBackend.gameIsInstalled

            Label{
                Layout.preferredWidth: 250

                Layout.leftMargin: 5
                Layout.rightMargin: 5

                font.pixelSize: 11
                text: qsTr("Active mod: ") + _uiBackend.balanceModPage.currentModInGame
                color: DowStatsStyle.textColor
            }

            StyledComboBox{
                id: modComboBox
                model: [qsTr("Original Soulstorm"), qsTr("DoW Stats Balance Mod (Latest)"), qsTr("Current active mod")]

                Layout.preferredWidth: 250
                Layout.preferredHeight: 25

                onObjectSelected: _uiBackend.settingsPageModel.launchMode = currentIndex

                Connections{
                    target: _uiBackend.settingsPageModel
                    function onLaunchModeChanged(){
                        modComboBox.currentIndex = _uiBackend.settingsPageModel.launchMode
                    }
                }
            }
        }*/

        Rectangle{
            id: lounchSoulstormButtonRectangle
            //width: 15 + lounchSoulstormLabel.width + 15 + iconImage.width + 5
            Layout.preferredWidth:  15 + lounchSoulstormLabel.width + 15 + iconImage.width + 5
            height: 35
            radius:5
            Layout.rightMargin: 15

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#337ab7"
                }

                GradientStop {
                    position: 1
                    color: "#245580"
                }
            }

            RowLayout
            {
                anchors.fill: parent
                spacing: 15

                Label{
                    id: lounchSoulstormLabel
                    text: qsTr("Play")
                    Layout.leftMargin: 15
                    color: lounchSoulstormMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                    font.pixelSize: 16
                }

                Rectangle
                {
                    width: 30
                    height: 30
                    color: "#00000000"

                    Layout.rightMargin: 15

                    Image{
                        id: iconImage
                        anchors.fill: parent
                        source: "qrc:/images/resources/icons/ssIcon.png"
                    }
                }
            }

            MouseArea{
                id: lounchSoulstormMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true

                onClicked: {
                    _uiBackend.launchGame()
                }
            }
        }
    }
}
