import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {

    id: itemRectangle

    property string playerName
    property int playerMmr
    property int playerMmr1v1
    property int playerGamesCount
    property string playerRace
    property int playerWinRate
    property int playerApm
    property bool playerIsBanned
    property bool playerVisible
    property string steamId

    property url avatarSource: ""

    property double sizeModifer: 1

    property var hoveredState

    Layout.maximumWidth: 280 * sizeModifer
    Layout.maximumHeight: 120 * sizeModifer
    Layout.minimumWidth: 280 * sizeModifer
    Layout.minimumHeight: 120 * sizeModifer
    height: 120 * sizeModifer
    Layout.alignment: Qt.AlignRight | Qt.AlignTop
    Layout.fillHeight: false
    Layout.fillWidth: true

    onPlayerGamesCountChanged:
    {
        changeRank();
    }

    onPlayerMmr1v1Changed:
    {
        changeRank();
    }

    function changeRank()
    {
        if (playerGamesCount < 20)
        {
            rankImage.source = "qrc:/images/resources/images/calibrate_60.png"
            return;
        }

        if (playerMmr1v1 < 1400)
        {
            rankImage.source = "qrc:/images/resources/images/rank0_60.png"
            return;
        }

        if (playerMmr1v1 < 1550)
        {
            rankImage.source = "qrc:/images/resources/images/rank1_60.png"
            return;
        }

        if (playerMmr1v1 < 1650)
        {
            rankImage.source = "qrc:/images/resources/images/rank2_60.png"
            return;
        }

        if (playerMmr1v1 < 1800)
        {
            rankImage.source = "qrc:/images/resources/images/rank3_60.png"
            return;
        }

        if (playerMmr1v1 < 2000)
        {
            rankImage.source = "qrc:/images/resources/images/rank4_60.png"
            return;
        }

        if (playerMmr1v1 < 2250)
        {
            rankImage.source = "qrc:/images/resources/images/rank5_60.png"
            return;
        }

        rankImage.source = "qrc:/images/resources/images/rank6_60.png"
        return;
    }


    color: itemRectangle.hoveredState ? "#c8c8c8" : ((itemRectangle.playerIsBanned) ? "#ffa9a9" : "#ffffff")
    radius: 10 * sizeModifer


    RowLayout {
        id: rowLayout
        anchors.fill: parent

        ColumnLayout {
            id: columnLayout

            Rectangle {
                id: rectangle

                color: "#00000000"
                radius: 10 * sizeModifer
                Layout.topMargin: (itemRectangle.playerIsBanned ? 10 : 0) * sizeModifer
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.maximumWidth: 80 * sizeModifer
                Layout.preferredHeight: 60 * sizeModifer
                Layout.preferredWidth: 80 * sizeModifer
                Layout.minimumHeight: 60 * sizeModifer
                Layout.minimumWidth: 80 * sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true


                Rectangle {
                    id: avatarRectangle

                    width: 60 * sizeModifer
                    height:60 * sizeModifer
                    radius: 10 * sizeModifer
                    //Layout.rightMargin: 15

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    Image {
                        id: avatarImage
                        cache: false
                        width: 60 * sizeModifer
                        height: 60 * sizeModifer
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: itemRectangle.avatarSource
                        visible:false
                        fillMode: Image.PreserveAspectFit
                    }

                     Rectangle {
                         id: maskRectangle

                         anchors.fill: parent
                         radius: 10 * sizeModifer
                         visible: false

                     }

                     OpacityMask {
                         id: opacityMask
                         anchors.fill: avatarImage
                         source: avatarImage
                         maskSource: maskRectangle
                     }
                }
            }

            Label {
                id: statusLabel
                visible: itemRectangle.playerIsBanned
                text: qsTr("[CHEATER]")
                font.pixelSize: 15 * sizeModifer

                Layout.bottomMargin: 10 * sizeModifer
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout {
            id: columnLayout4
            spacing: 5 * sizeModifer
            Layout.margins: 5 * sizeModifer
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.bottomMargin: 0

            Label {
                id: label1
                text: itemRectangle.playerName
                Layout.maximumWidth: 180 * sizeModifer

                font.pixelSize: 20 * sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout
            {

                ColumnLayout
                {

                    spacing: 1

                    Label {
                        id: label2
                        text: "MMR: " + (itemRectangle.playerGamesCount < 20 ? "Calibrate" : itemRectangle.playerMmr)
                        font.pixelSize: 12 * sizeModifer

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        id: label
                        text: "Solo MMR: " + (itemRectangle.playerGamesCount < 20 ? "Calibrate" : itemRectangle.playerMmr1v1)
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        font.pixelSize: 12 * sizeModifer
                    }

                    Label {
                        id: label4
                        text: "Games played: " + itemRectangle.playerGamesCount
                        font.pixelSize: 12 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Label {
                        id: label3
                        text: "Race: " + itemRectangle.playerRace
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        font.pixelSize: 12 * sizeModifer
                    }

                    Label {
                        id: label5
                        text: "Win rate: " + itemRectangle.playerWinRate + "%"
                        font.pixelSize: 12 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Label {
                        id: label6
                        text: "APM: " + itemRectangle.playerApm
                        font.pixelSize: 12 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }
                }


                Rectangle
                {
                    Layout.maximumWidth: 60 * sizeModifer
                    Layout.preferredHeight: 60 * sizeModifer
                    Layout.preferredWidth: 60 * sizeModifer
                    Layout.minimumHeight: 60 * sizeModifer
                    Layout.minimumWidth: 60 * sizeModifer
                    Layout.margins: 5 * sizeModifer

                    //color: "#b7b7b7"
                    color: "#00000000"
                    radius:5

                    Image
                    {
                        id: rankImage

                        cache: false
                        width: 50 * sizeModifer
                        height: 50 * sizeModifer
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        fillMode: Image.PreserveAspectFit

                        source: "qrc:/images/resources/images/rank1_60.png"
                    }
                }
            }
        }
    }

    MouseArea{
        id: playerItemMouseArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            Qt.openUrlExternally("https://dowstats.ru/player.php?sid=" + itemRectangle.steamId + "&server=steam");
        }

        onContainsMouseChanged: {
            if (playerItemMouseArea.containsMouse)
                //itemRectangle.color = "#c8c8c8";
                itemRectangle.hoveredState = true;
            else
                //itemRectangle.color = (itemRectangle.playerIsBanned) ? "#ffa9a9" : "#ffffff"
                itemRectangle.hoveredState = false;
        }
    }
}
