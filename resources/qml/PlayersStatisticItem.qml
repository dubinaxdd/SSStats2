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

    property bool relicStatsAvailable: false
    property string relicCountry: ""
    property int relicGamesCount: 0
    property int relicWinCount: 0
    property int rating_1x1: 0
    property int rating_2x2: 0
    property int rating_3x3: 0
    property string race_1x1: ""
    property string race_2x2: ""
    property string race_3x3: ""

    height: contentColumnLayout.height
    width: 280 * sizeModifer

    Layout.preferredWidth: 280 * sizeModifer
    Layout.preferredHeight: contentColumnLayout.height

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

    ColumnLayout
    {
        id: contentColumnLayout
        //anchors.fill: parent
        spacing: 5 * sizeModifer

        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout {
            id: rowLayout
            Layout.fillWidth: true
            spacing: 0

            ColumnLayout {
                id: avatarColumnLayout

                Layout.topMargin: 10 * sizeModifer
                Layout.bottomMargin: 10 * sizeModifer
                spacing: 5 * sizeModifer

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

                Item{
                    visible: !root.relicStatsAvailable || root.banType === BanType.Banned || root.banType === BanType.SoftwareUseBan || root.banType === BanType.Cheater
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

                Item{
                    visible: root.relicStatsAvailable
                    Layout.fillHeight: true
                }
            }

            ColumnLayout {
                id: statisticColumnLayout
                spacing: 3 * sizeModifer
                Layout.leftMargin: 0
                Layout.rightMargin: 5 * sizeModifer
                Layout.topMargin: 5 * sizeModifer
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
                    Layout.fillWidth: true

                    ColumnLayout
                    {
                        spacing: 0
                        Layout.fillWidth: true

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
                            visible: root.customGamesMmr != 0
                            text: qsTr("Custom games MMR: ") + root.customGamesMmr
                            font.pixelSize: 11 * sizeModifer
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: root.textColor
                        }

                        Label {
                            id: label4
                            text: qsTr("Games (Winrate): ") + root.playerGamesCount + " (" + root.playerWinRate + "%)"
                            font.pixelSize: 11 * sizeModifer
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: root.textColor
                        }

                        Label {
                            id: label3
                            visible: root.playerRace !== ""
                            text: qsTr("Race: ") + root.playerRace
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.pixelSize: 11 * sizeModifer
                            color: root.textColor
                        }

                        /*Label {
                            id: label5
                            text: qsTr("Wins rate: ") + root.playerWinRate + "%"
                            font.pixelSize: 11 * sizeModifer
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: root.textColor
                        }*/

                        Label {
                            id: label6
                            visible: !root.relicStatsAvailable
                            text: "APM: " + root.playerApm
                            font.pixelSize: 11 * sizeModifer
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            color: root.textColor
                        }
                    }

                    ColumnLayout
                    {
                        Layout.preferredWidth: 50 * sizeModifer
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

                Item{
                    Layout.fillHeight: true
                    visible: !root.relicStatsAvailable
                }

                ColumnLayout
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: root.relicStatsAvailable
                    spacing: 1 * sizeModifer

                    Label {
                        id: relicLadderLabel
                        visible: root.relicStatsAvailable
                        text: "Relic Ladder"
                        font.pixelSize: 13 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.topMargin: 5 * sizeModifer
                        color: root.textColor
                    }

                    Label {
                        id: rating_1x1Label
                        visible: root.relicStatsAvailable && root.rating_1x1 > 0
                        text: "Rating 1x1: " + root.rating_1x1 + " (" + root.race_1x1 + ")"
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    Label {
                        id: rating_2x2Label
                        visible: root.relicStatsAvailable && root.rating_2x2 > 0
                        text: "Rating 2x2: " + root.rating_2x2 + " (" + root.race_2x2 + ")"
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    Label {
                        id: rating_3x3Label
                        visible: root.relicStatsAvailable && root.rating_3x3 > 0
                        text: "Rating 3x3: " + root.rating_3x3 + " (" + root.race_3x3 + ")"
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    Label {
                        id: relicGamesCountLabel
                        visible: root.relicStatsAvailable
                        text: qsTr("Games (Winrate): ") + root.relicGamesCount + " (" + (root.relicGamesCount >0 ? Math.trunc((root.relicWinCount/root.relicGamesCount) * 100) : 0)+ "%)"
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }

                    /*Label {
                        id: relicWinCountLabel
                        visible: root.relicStatsAvailable
                        text: "Win count: " + root.relicWinCount
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }*/

                   /* Label {
                        id: relicCountryLabel
                        visible: root.relicStatsAvailable
                        text: "Country: " + root.relicCountry
                        font.pixelSize: 11 * sizeModifer
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: root.textColor
                    }*/

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
