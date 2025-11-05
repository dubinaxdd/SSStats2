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

        relicStatsAvailable: root.model.curentPlayerStatsItem.relicStatsAvailable
        relicCountry: root.model.curentPlayerStatsItem.relicCountry
        relicGamesCount: root.model.curentPlayerStatsItem.relicGamesCount
        relicWinCount: root.model.curentPlayerStatsItem.relicWinCount
        rating_1x1: root.model.curentPlayerStatsItem.rating_1x1
        rating_2x2: root.model.curentPlayerStatsItem.rating_2x2
        rating_3x3: root.model.curentPlayerStatsItem.rating_3x3
        race_1x1: root.model.curentPlayerStatsItem.race_1x1
        race_2x2: root.model.curentPlayerStatsItem.race_2x2
        race_3x3: root.model.curentPlayerStatsItem.race_3x3

        avatarSource: "image://imageprovider/currentPlayerAvatarMedium"
        sizeModifer: root.sizeModifer
        hoverEnabled: root.hoverEnabled
    }

    ListView
    {
        id: playersListView

        model: root.model

        Layout.preferredHeight: contentHeight
        Layout.maximumHeight: contentHeight
        Layout.minimumHeight: contentHeight

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

            relicStatsAvailable: model.relicStatsAvailable
            relicCountry: model.relicCountry
            relicGamesCount: model.relicGamesCount
            relicWinCount: model.relicWinCount
            rating_1x1: model.rating_1x1
            rating_2x2: model.rating_2x2
            rating_3x3: model.rating_3x3
            race_1x1: model.race_1x1
            race_2x2: model.race_2x2
            race_3x3: model.race_3x3

            avatarSource: "image://imageprovider/" + model.avatarId
            sizeModifer: root.sizeModifer
            hoverEnabled: root.hoverEnabled
        }
    }
}
