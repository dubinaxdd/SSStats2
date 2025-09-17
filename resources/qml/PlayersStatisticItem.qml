import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0
import BanType 1.0

Rectangle {

    id: root

    property string playerName
    property int playerMmr
    property int playerMmr1v1
    property int customGamesMmr
    property int playerGamesCount
    property int playerRank
    property string playerRace
    property int playerWinRate
    property int playerApm
    property bool playerIsBanned
    property bool playerVisible
    property string steamId
    //property int calibrateGamesLeft
    property int banType
    property bool isOnline: false
    property bool isRanked: false

    property url avatarSource: ""

    property double sizeModifer: 1

    property var hoveredState
    property bool hoverEnabled: true

    property var textColor: root.hoveredState ? DowStatsStyle.textColor : (playerIsBanned ? "#26282a" : DowStatsStyle.textColor)

    Layout.maximumWidth: 280 * sizeModifer
    Layout.maximumHeight: 130 * sizeModifer
    Layout.minimumWidth: 280 * sizeModifer
    Layout.minimumHeight: 130 * sizeModifer
    height: 130 * sizeModifer
    Layout.alignment: Qt.AlignRight | Qt.AlignTop
    Layout.fillHeight: false
    Layout.fillWidth: true

    color: {
        if (root.hoverEnabled && root.hoveredState)
            return DowStatsStyle.itemColor
        else
        {
            if (root.playerIsBanned)
            {
                if (root.banType === BanType.FormerCheater)
                    return "#FAF884"
                else
                    return "#ffa9a9"
            }
            else
                return DowStatsStyle.backgroundColor
        }
    }

    radius: 10 * sizeModifer

    onPlayerGamesCountChanged:{
        changeRank();
    }

    onPlayerRankChanged:{
        changeRank();
    }

    function changeRank()
    {
        switch(root.playerRank){
            case(1): rankImage.source = "qrc:/images/resources/images/Rank8.svg"; return;
            case(2): rankImage.source = "qrc:/images/resources/images/Rank7.svg"; return;
            case(3): rankImage.source = "qrc:/images/resources/images/Rank6.svg"; return;
            case(4): rankImage.source = "qrc:/images/resources/images/Rank5.svg"; return;
            case(5): rankImage.source = "qrc:/images/resources/images/Rank4.svg"; return;
            case(6): rankImage.source = "qrc:/images/resources/images/Rank3.svg"; return;
            case(7): rankImage.source = "qrc:/images/resources/images/Rank1.svg"; return;
        }

        rankImage.source = "qrc:/images/resources/images/Rank2.svg"
        return;
    }

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        spacing: 5 * sizeModifer

        ColumnLayout {
            id: columnLayout

            Layout.topMargin: 10 * sizeModifer
            Layout.bottomMargin: 10 * sizeModifer
            spacing: 5 * sizeModifer

            Rectangle{
                Layout.fillHeight: true
                visible: root.banType === BanType.Banned || root.banType === BanType.Cheater || root.banType === BanType.SoftwareUseBan
            }

            Rectangle {
                id: rectangle

                color: "#00000000"
                radius: 10 * sizeModifer
                Layout.topMargin: 0

                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.maximumWidth: 80 * sizeModifer
                Layout.preferredHeight: 60 * sizeModifer
                Layout.preferredWidth: 80 * sizeModifer
                Layout.minimumHeight: 60 * sizeModifer
                Layout.minimumWidth: 80 * sizeModifer

                Rectangle {
                    id: avatarRectangle

                    width: 60 * sizeModifer
                    height:60 * sizeModifer
                    radius: 10 * sizeModifer

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    Image {
                        id: avatarImage
                        cache: false
                        width: 60 * sizeModifer
                        height: 60 * sizeModifer
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: root.avatarSource
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
                visible: root.banType === BanType.Cheater
                text: qsTr("[CHEATER]")
                font.pixelSize: 15 * sizeModifer
                Layout.alignment: Qt.AlignHCenter
                color: root.textColor
            }

            Label {
                id: statusLabel2
                visible: root.banType === BanType.Banned || root.banType === BanType.SoftwareUseBan
                text: qsTr("[BANNED]")
                font.pixelSize: 15 * sizeModifer
                Layout.alignment: Qt.AlignHCenter
                color: root.textColor
            }

            Rectangle{
                Layout.fillHeight: true
            }

            RowLayout
            {
                Layout.leftMargin: 10 * sizeModifer
                visible: !(root.banType === BanType.Banned || root.banType === BanType.Cheater || root.banType === BanType.SoftwareUseBan)

                Rectangle{
                    radius:5 * sizeModifer

                    Layout.preferredHeight: 10 * sizeModifer
                    Layout.preferredWidth: 10 * sizeModifer

                    color:  root.isOnline ? "#00ff99" : "#ffa9a9"
                }

                Label{
                    text: root.isOnline ? qsTr("Online") : qsTr("Offline")
                    font.pixelSize: 11 * sizeModifer
                    color: root.textColor
                }
            }


            RowLayout
            {
                Layout.leftMargin: 10 * sizeModifer

                visible: !(root.banType === BanType.Banned || root.banType === BanType.Cheater || root.banType === BanType.SoftwareUseBan)

                Rectangle{
                    radius:5 * sizeModifer

                    Layout.preferredHeight: 10 * sizeModifer
                    Layout.preferredWidth: 10 * sizeModifer

                    color:  root.isRanked ? "#00ff99" : "#ffa9a9"
                }

                Label{
                    text: root.isRanked ? qsTr("Ranked") : qsTr("Unranked")
                    font.pixelSize: 11 * sizeModifer
                    color: root.textColor
                }
            }
        }

        ColumnLayout {
            id: columnLayout4
            spacing: 3 * sizeModifer
            Layout.margins: 5 * sizeModifer

            Layout.leftMargin: 0
            Layout.bottomMargin: 10 * sizeModifer
            Layout.fillHeight: true
            Layout.fillWidth: true


            Label {
                id: label1
                text: root.playerName
                Layout.maximumWidth: 180 * sizeModifer
                color: root.textColor

                font.pixelSize: 20 * sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout
            {

                ColumnLayout
                {

                    spacing: 0


                    Label {
                        id: label
                        text: qsTr("Solo MMR: ") + root.playerMmr1v1
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        font.pixelSize: 11 * sizeModifer
                        color: root.textColor
                    }

                    Label {
                        id: label2
                        text: qsTr("Team MMR: ") + root.playerMmr
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: root.textColor
                    }

                    Label {
                        id: customGameslabel
                        text: qsTr("Custom games MMR: ") + root.customGamesMmr
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: root.textColor
                    }

                    Label {
                        id: label4
                        text: qsTr("Games played: ") + root.playerGamesCount
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    Label {
                        id: label3
                        text: qsTr("Race: ") + root.playerRace
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        font.pixelSize: 11 * sizeModifer
                        color: root.textColor
                    }

                    Label {
                        id: label5
                        text: qsTr("Wins rate: ") + root.playerWinRate + "%"
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    Label {
                        id: label6
                        text: "APM: " + root.playerApm
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }
                }

                ColumnLayout
                {
                    spacing:0
                    Layout.rightMargin: 5 * sizeModifer

                    Rectangle
                    {
                        Layout.maximumWidth: 50 * sizeModifer
                        Layout.preferredHeight: 50 * sizeModifer
                        Layout.preferredWidth: 50 * sizeModifer
                        Layout.minimumHeight: 50 * sizeModifer
                        Layout.minimumWidth: 50 * sizeModifer
                        Layout.margins: 0
                        color: "#00000000"

                        Image
                        {
                            id: rankImage

                            cache: false
                            width: 50 * sizeModifer
                            height: 50 * sizeModifer
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            fillMode: Image.PreserveAspectFit

                            source: "qrc:/images/resources/images/Rank2.svg"
                            sourceSize.width: 60 * sizeModifer
                            sourceSize.height: 60 * sizeModifer
                        }
                    }

                    Label {
                        visible: root.banType === BanType.FormerCheater
                        text: qsTr("Former") + "\n" + qsTr("cheater")
                        font.pixelSize: 10 * sizeModifer
                        Layout.alignment: Qt.AlignHCenter
                        color: root.textColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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
            Qt.openUrlExternally("https://dowstats.ru/player.php?sid=" + root.steamId + "&mod_tech_name=" + _uiBackend.currentModTechnicalName);
        }

        onContainsMouseChanged: {
            if (playerItemMouseArea.containsMouse)
                root.hoveredState = true;
            else
                root.hoveredState = false;
        }
    }
}
