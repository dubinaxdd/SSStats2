import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: mainRectangle

    width: 280
    Layout.minimumWidth: 280
    Layout.maximumWidth: 280

    visible: model.curentPlayerStatsItem.itemVisible && model.curentPlayerStatsItem.playerName !== ""

    color: "#00000000"
    border.color: "#00000000"

    property var model

    //Костыль для перезагрузки картинки, рил так на формух делают
    Connections {
        target: model

        function onCurrentPlayerStatsChanged()
        {
            var oldSource = curentPlayer.avatarSource;
            curentPlayer.avatarSource = "";
            curentPlayer.avatarSource = oldSource;
        }
    }

    ScrollView {
        id: scrollView

        height: mainRectangle.height
        width: mainRectangle.width

        clip: true

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent
            spacing: 5

            PlayersStatisticItem
            {
                id:curentPlayer
                visible: model.curentPlayerStatsItem.itemVisible && model.curentPlayerStatsItem.playerName !== ""

                playerName: model.curentPlayerStatsItem.playerName
                playerMmr: model.curentPlayerStatsItem.playerMmr
                playerMmr1v1: model.curentPlayerStatsItem.playerMmr1v1
                playerGamesCount: model.curentPlayerStatsItem.playerGamesCount
                playerRace: model.curentPlayerStatsItem.playerRace
                playerWinRate: model.curentPlayerStatsItem.playerWinRate
                playerApm: model.curentPlayerStatsItem.playerApm
                playerIsBanned: model.curentPlayerStatsItem.playerIsBanned
                playerVisible: model.curentPlayerStatsItem.itemVisible

                avatarSource: "image://imageprovider/currentPlayerAvatarMedium"

            }

            ListView
            {
                id: playersListView
                model:_uiBackend.statisticPanel

                Layout.preferredHeight: ((120 + 5) * count  - 5)
                Layout.maximumHeight: ((120 + 5) * count - 5)
                Layout.minimumHeight: ((120 + 5) * count - 5)

                spacing: 5

                delegate: PlayersStatisticItem{

                    playerName: model.playerName
                    playerMmr: model.playerMmr
                    playerMmr1v1: model.playerMmr1v1
                    playerGamesCount: model.playerGamesCount
                    playerRace: model.playerRace
                    playerWinRate: model.playerWinRate
                    playerApm: model.playerApm
                    playerIsBanned: model.playerIsBanned
                    playerVisible: model.playerVisible

                    avatarSource: "image://imageprovider/" + model.avatarId

                    height: 120
                    width: 280
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}


