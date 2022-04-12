import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: mainRectangle

    width: 280 * _uiBackend.sizeModifer

    Layout.minimumWidth: 280 * _uiBackend.sizeModifer
    Layout.maximumWidth: 280 * _uiBackend.sizeModifer

    color: "#00000000"
    border.color: "#00000000"

    property var model
    property real scrollViewPosition: 0
    property int playersCount: 1 + playersListView.count
    property int relativeMouseX
    property int relativeMouseY

    function mouseClick(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        // Кнопка "Свернуть колонку статистики"
        if (relativeMouseX >= expandPatyStatisticButtonRectangle.x &&
            relativeMouseX <= expandPatyStatisticButtonRectangle.x + expandPatyStatisticButtonRectangle.width &&
            relativeMouseY >= expandPatyStatisticButtonRectangle.y - (expandPatyStatisticButtonRectangle.y * scrollViewPosition) &&
            relativeMouseY <= expandPatyStatisticButtonRectangle.y - (expandPatyStatisticButtonRectangle.y * scrollViewPosition) + expandPatyStatisticButtonRectangle.height)
        {
            if(!_uiBackend.gamePanel.gamePanelVisible && expandPatyStatisticButtonRectangle.visible == true)
            {
                expandPatyStatisticButtonRectangle.howeredState = true;
                _uiBackend.expandPatyStatisticButtonClick();

               scrollView.setDefault();
            }
        }
    }

    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y

        //updateScrollViewHeight();

        // Кнопка "Свернуть колонку статистики"
        if (relativeMouseX >= expandPatyStatisticButtonRectangle.x &&
            relativeMouseX <= expandPatyStatisticButtonRectangle.x + expandPatyStatisticButtonRectangle.width &&
            relativeMouseY >= expandPatyStatisticButtonRectangle.y - (expandPatyStatisticButtonRectangle.y * scrollViewPosition) &&
            relativeMouseY <= expandPatyStatisticButtonRectangle.y - (expandPatyStatisticButtonRectangle.y * scrollViewPosition) + expandPatyStatisticButtonRectangle.height)
        {

            if(!expandPatyStatisticButtonRectangle.howeredState)
                expandPatyStatisticButtonRectangle.howeredState = true;
        }
        else
        {
            if(expandPatyStatisticButtonRectangle.howeredState)
                expandPatyStatisticButtonRectangle.howeredState = false;
        }
    }

    function mouseWheel(delta)
    {
        //updateScrollViewHeight();

        if (relativeMouseX >= scrollView.x &&
            relativeMouseX <= scrollView.x + scrollView.width &&
            relativeMouseY >= scrollView.y &&
            relativeMouseY <= scrollView.y + scrollView.height
                )
        {

            if (delta > 0)
                scrollView.scrollToTop();

            if (delta < 0)
                scrollView.scrollToBottom()
        }
    }

/*
    function updateScrollViewHeight()
    {
        var playersCountTemp = playersCount;

        if (playersCountTemp !== playersCount)
        {
            scrollView.setDefault();
            scrollView.height = ((120 * _uiBackend.sizeModifer) + columnLayout.spacing) * playersCount ;
        }
    }


    function forceUpdateScrollViewHeight()
    {
        scrollView.setDefault();
        scrollView.height = ((120 * _uiBackend.sizeModifer) + columnLayout.spacing) * playersCount ;
    }

    Connections{
        target: _uiBackend.statisticPanel

        function onPlayersStatsChanged()
        {
            forceUpdateScrollViewHeight();
        }
    }

    Connections{
        target: _uiBackend

        function onSizeModiferChanged(sizeModifer)
        {
            updateScrollViewHeight();
        }

        function onSizeModiferLoadedFromSettings(sizeModifer)
        {
            updateScrollViewHeight();
        }

        function onExpandButtonPressed()
        {
            forceUpdateScrollViewHeight();
        }
    }*/

    //Костыль для перезагрузки картинки, рил так на формух делают

    Connections {
        target: model

        function onCurrentPlayerStatsChanged()
        {
            var oldSource = curentPlayer.avatarSource;
            curentPlayer.avatarSource = "";
            curentPlayer.avatarSource = oldSource;
        }

        function onPlayersStatsChanged()
        {
            var oldSource = player2.avatarSource;
            player2.avatarSource = "";
            player2.avatarSource = oldSource;

            oldSource = player3.avatarSource;
            player3.avatarSource = "";
            player3.avatarSource = oldSource;

            oldSource = player4.avatarSource;
            player4.avatarSource = "";
            player4.avatarSource = oldSource;

            oldSource = player5.avatarSource;
            player5.avatarSource = "";
            player5.avatarSource = oldSource;

            oldSource = player6.avatarSource;
            player6.avatarSource = "";
            player6.avatarSource = oldSource;

            oldSource = player7.avatarSource;
            player7.avatarSource = "";
            player7.avatarSource = oldSource;

            oldSource = player8.avatarSource;
            player8.avatarSource = "";
            player8.avatarSource = oldSource;
        }
    }

    ScrollView {
        id: scrollView

        height: mainRectangle.height
        width: mainRectangle.width

        function scrollToBottom() {

            if (ScrollBar.vertical.position + 0.125  < 0.9 && (!model.expandPatyStatistic || _uiBackend.expand))
                ScrollBar.vertical.position += 0.125

            scrollViewPosition = ScrollBar.vertical.position
        }

        function scrollToTop() {

            if (ScrollBar.vertical.position - 0.125  >= 0 && (!model.expandPatyStatistic || _uiBackend.expand))
                ScrollBar.vertical.position -= 0.125

            scrollViewPosition = ScrollBar.vertical.position
        }

        function setDefault()
        {
            ScrollBar.vertical.position = 0
            scrollViewPosition = ScrollBar.vertical.position
        }

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent

            spacing: 5 * _uiBackend.sizeModifer


            PlayersStatisticItem
            {
                id:curentPlayer
                visible: model.curentPlayerStatsItem.itemVisible && (!model.expandPatyStatistic || _uiBackend.expand) && model.curentPlayerStatsItem.playerName !== ""

                playerName: model.curentPlayerStatsItem.playerName
                playerMmr: model.curentPlayerStatsItem.playerMmr
                playerMmr1v1: model.curentPlayerStatsItem.playerMmr1v1
                playerGamesCount: model.curentPlayerStatsItem.playerGamesCount
                playerRace: model.curentPlayerStatsItem.playerRace
                playerWinRate: model.curentPlayerStatsItem.playerWinRate
                playerPlayerApm: model.curentPlayerStatsItem.playerApm
                playerIsBanned: model.curentPlayerStatsItem.playerIsBanned
                playerVisible: model.curentPlayerStatsItem.itemVisible

                avatarSource: "image://ImageProvider/currentPlayerAvatarMedium"

            }

            ListView
            {
                id: playersListView

                model:_uiBackend.statisticPanel

                Layout.preferredHeight: (120 + 5) * count * _uiBackend.sizeModifer
                Layout.maximumHeight: (120 + 5) * count * _uiBackend.sizeModifer
                Layout.minimumHeight: (120 + 5) * count * _uiBackend.sizeModifer

                spacing: 5 * _uiBackend.sizeModifer

                delegate: PlayersStatisticItem{

                    playerName: model.playerName
                    playerMmr: model.playerMmr
                    playerMmr1v1: model.playerMmr1v1
                    playerGamesCount: model.playerGamesCount
                    playerRace: model.playerRace
                    playerWinRate: model.playerWinRate
                    playerPlayerApm: model.playerApm
                    playerIsBanned: model.playerIsBanned
                    playerVisible: model.playerVisible

                    height: 120 * _uiBackend.sizeModifer
                    width: 280 * _uiBackend.sizeModifer
                }
            }


/*

            PlayersStatisticItem
            {
                id:player2
                visible: model.player2StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player2StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player2StatsItem
                avatarSource: "image://ImageProvider/player2AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player3
                visible: model.player3StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player3StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player3StatsItem
                avatarSource: "image://ImageProvider/player3AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player4
                visible: model.player4StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player4StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player4StatsItem
                avatarSource: "image://ImageProvider/player4AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player5
                visible: model.player5StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player5StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player5StatsItem
                avatarSource: "image://ImageProvider/player5AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player6
                visible: model.player6StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player6StatsItem.playerName !== ""
               // visible: true
                itemModel: model.player6StatsItem
                avatarSource: "image://ImageProvider/player6AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player7
                visible: model.player7StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player7StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player7StatsItem
                avatarSource: "image://ImageProvider/player7AvatarMedium"

            }


            PlayersStatisticItem
            {
                id:player8
                visible: model.player8StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player8StatsItem.playerName !== ""
                //visible: true
                itemModel: model.player8StatsItem
                avatarSource: "image://ImageProvider/player8AvatarMedium"
            }
*/

            Rectangle {
                id: rectangle1
                visible: _uiBackend.ssWindowed && !_uiBackend.headerVisible
                color: "#00000000"
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: 25 * _uiBackend.sizeModifer
                Layout.maximumHeight: 25 * _uiBackend.sizeModifer
                Layout.minimumHeight: 25 * _uiBackend.sizeModifer
                height: 25 * _uiBackend.sizeModifer
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
            }

            Rectangle {
                id: expandPatyStatisticButtonRectangle

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

                height: 10 * _uiBackend.sizeModifer
                radius: 5 * _uiBackend.sizeModifer

                Layout.maximumHeight: 10 * _uiBackend.sizeModifer
                Layout.minimumHeight: 10 * _uiBackend.sizeModifer
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: false
                visible: !_uiBackend.expand


                gradient: howeredState ? grDark : grLight

                Image {
                    id: image
                    width: 95 * _uiBackend.sizeModifer
                    height: 10 * _uiBackend.sizeModifer
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/images/resources/images/expandDots.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}


