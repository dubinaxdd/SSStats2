import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Rectangle {
    width:50

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
            Layout.fillWidth: true
        }
    }
}
