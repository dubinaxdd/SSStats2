import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Rectangle {
    width: 50
    color: "#ffffff"

    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#FFFFFF"
        }

        GradientStop {
            position: 1
            color: "#C0C0C0"
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
                    Qt.openUrlExternally("https://discordapp.com/channels/606832876369215491/852182655570411592");
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
            Layout.fillWidth: true
        }

        Rectangle{
            id: lounchSoulstormButtonRectangle
            width: 15 + lounchSoulstormLabel.width + 15 + iconImage.width + 5
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
                    text: "Launch Soulstorm"
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
                    _uiBackend.launchSoulstorm()
                }
            }
        }
    }
}
