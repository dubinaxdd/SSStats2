import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import DowStatsStyle 1.0
import GameType 1.0

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
        anchors.leftMargin: 15

        spacing: 15

        LinkButton
        {
            text: "dowstats.ru"
            link: "https://dowstats.ru/"
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

                onClicked: Qt.openUrlExternally("https://discord.gg/t7WqcDDeSU");
            }
        }

        LinkButton
        {
            text: "DoW Player's Guide"
            link: "https://warhammer-guide.ru/wiki/Dawn_of_War_Players_Guide.html"
        }

        LinkButton
        {
            text: "DowOnline"
            link: "https://dowonline.ru"
            iconSource: "qrc:/images/resources/images/dow_online.png"
        }

        LinkButton
        {
            text: "Relic Ladder"
            link: "https://dowstats.com/relic-ladder"
        }

        LinkButton
        {
            text: "DoW Wiki"
            link: "http://wiki.dawn-of-war.pro/"
        }

        LinkButton
        {
            text: qsTr("Help")
            link: "https://dowstats.ru/support.php"
        }

        Rectangle{
            Layout.fillWidth: true
        }

        Rectangle{
            id: lounchSoulstormButtonRectangle
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
                        source: _uiBackend.gamePage.currentGameType  !==  GameType.DefinitiveEdition ? "qrc:/images/resources/icons/ssIcon.png" : "qrc:/icons/resources/icons/DefinitiveEdition.png"
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
