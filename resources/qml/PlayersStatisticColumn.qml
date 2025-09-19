import QtQuick 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: root
    spacing: 5 * root.sizeModifer

    property double sizeModifer: 1.0
    property bool hoverEnabled
    property var model: _uiBackend.statisticPanel
    property int plyersCount: playersListView.count + 1

    //Костыль для перезагрузки картинки, рил так на формух делают
    Connections {
        target: root.model

        function onCurrentPlayerStatsChanged()
        {
            var oldSource = curentPlayer.avatarSource;
            curentPlayer.avatarSource = "";
            curentPlayer.avatarSource = oldSource;
        }
    }

    PlayersStatisticItem
    {
        id:curentPlayer
        visible: root.model.curentPlayerStatsItem.itemVisible && root.model.curentPlayerStatsItem.playerName !== ""

        playerName: root.model.curentPlayerStatsItem.playerName
        playerMmr: root.model.curentPlayerStatsItem.playerMmr
        playerMmr1v1: root.model.curentPlayerStatsItem.playerMmr1v1
        customGamesMmr: model.curentPlayerStatsItem.customGamesMmr
        playerGamesCount: root.model.curentPlayerStatsItem.playerGamesCount
        playerRank: root.model.curentPlayerStatsItem.playerRank
        playerRace: root.model.curentPlayerStatsItem.playerRace
        playerWinRate: root.model.curentPlayerStatsItem.playerWinRate
        playerApm: root.model.curentPlayerStatsItem.playerApm
        playerIsBanned: root.model.curentPlayerStatsItem.playerIsBanned
        playerVisible: root.model.curentPlayerStatsItem.itemVisible
        banType: root.model.curentPlayerStatsItem.banType
        //calibrateGamesLeft: model.curentPlayerStatsItem.calibrateGamesLeft
        isRanked: root.model.curentPlayerStatsItem.isRanked
        isOnline: root.model.curentPlayerStatsItem.isOnline
        steamId: root.model.curentPlayerStatsItem.steamId

        avatarSource: "image://imageprovider/currentPlayerAvatarMedium"
        sizeModifer: root.sizeModifer
        hoverEnabled: root.hoverEnabled

    }

    ListView
    {
        id: playersListView

        model: root.model//_uiBackend.statisticPanel

        Layout.preferredHeight: ((130 + 5) * count  - 5) * root.sizeModifer
        Layout.maximumHeight: ((130 + 5) * count - 5) * root.sizeModifer
        Layout.minimumHeight: ((130 + 5) * count - 5) * root.sizeModifer

        spacing: 5 * root.sizeModifer

        delegate: PlayersStatisticItem{

            playerName: model.playerName
            playerMmr: model.playerMmr
            playerMmr1v1: model.playerMmr1v1
            customGamesMmr: model.customGamesMmr
            playerGamesCount: model.playerGamesCount
            playerRank: model.playerRank
            playerRace: model.playerRace
            playerWinRate: model.playerWinRate
            playerApm: model.playerApm
            playerIsBanned: model.playerIsBanned
            playerVisible: model.playerVisible
            banType: model.banType
            //calibrateGamesLeft: model.calibrateGamesLeft
            isRanked: model.isRanked
            isOnline: model.isOnline
            steamId: model.steamId

            avatarSource: "image://imageprovider/" + model.avatarId
            sizeModifer: root.sizeModifer
            hoverEnabled: root.hoverEnabled

            height: 130 * root.sizeModifer
            width: 280 * root.sizeModifer
        }
    }
}
