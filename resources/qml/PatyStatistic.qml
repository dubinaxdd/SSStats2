import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    property var model
    width: 260
    height: 880
    color: "#00000000"
    border.color: "#00000000"

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

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        PlayerStatistic
        {
            id:curentPlayer
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.currentPlayerIsBanned

            avatarSource: "image://ImageProvider/currentPlayerAvatarMedium"
            name: model.currentPlayerName
            mmr: model.currentPlayerMmr
            mmr1v1: model.currentPlayerMmr1v1
            gamesCount: model.currentPlayerGamesCount
            race: model.currentPlayerRace
            winRate: model.currentPlayerWinRate
            apm: model.currentPlayerApm
        }

        PlayerStatistic
        {
            id:player2
            visible: model.player2StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player2IsBanned

            avatarSource: "image://ImageProvider/player2AvatarMedium"
            name: model.player2Name
            mmr: model.player2Mmr
            mmr1v1: model.player2Mmr1v1
            gamesCount: model.player2GamesCount
            race: model.player2Race
            winRate: model.player2WinRate
            apm: model.player2Apm
        }

        PlayerStatistic
        {
            id:player3
            visible: model.player3StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player3IsBanned

            avatarSource: "image://ImageProvider/player3AvatarMedium"
            name: model.player3Name
            mmr: model.player3Mmr
            mmr1v1: model.player3Mmr1v1
            gamesCount: model.player3GamesCount
            race: model.player3Race
            winRate: model.player3WinRate
            apm: model.player3Apm
        }

        PlayerStatistic
        {
            id:player4
            visible: model.player4StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player4IsBanned

            avatarSource: "image://ImageProvider/player4AvatarMedium"
            name: model.player4Name
            mmr: model.player4Mmr
            mmr1v1: model.player4Mmr1v1
            gamesCount: model.player4GamesCount
            race: model.player4Race
            winRate: model.player4WinRate
            apm: model.player4Apm
        }

        PlayerStatistic
        {
            id:player5
            visible: model.player5StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player5IsBanned

            avatarSource: "image://ImageProvider/player5AvatarMedium"
            name: model.player5Name
            mmr: model.player5Mmr
            mmr1v1: model.player5Mmr1v1
            gamesCount: model.player5GamesCount
            race: model.player5Race
            winRate: model.player5WinRate
            apm: model.player5Apm
        }

        PlayerStatistic
        {
            id:player6
            visible: model.player6StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player6IsBanned

            avatarSource: "image://ImageProvider/player6AvatarMedium"
            name: model.player6Name
            mmr: model.player6Mmr
            mmr1v1: model.player6Mmr1v1
            gamesCount: model.player6GamesCount
            race: model.player6Race
            winRate: model.player6WinRate
            apm: model.player6Apm
        }

        PlayerStatistic
        {
            id:player7
            visible: model.player7StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player7IsBanned


            avatarSource: "image://ImageProvider/player7AvatarMedium"
            name: model.player7Name
            mmr: model.player7Mmr
            mmr1v1: model.player7Mmr1v1
            gamesCount: model.player7GamesCount
            race: model.player7Race
            winRate: model.player7WinRate
            apm: model.player7Apm
        }


        PlayerStatistic
        {
            id:player8
            visible: model.player8StatsVisible

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            isBanned: model.player8IsBanned


            avatarSource: "image://ImageProvider/player8AvatarMedium"
            name: model.player8Name
            mmr: model.player8Mmr
            mmr1v1: model.player8Mmr1v1
            gamesCount: model.player8GamesCount
            race: model.player8Race
            winRate: model.player8WinRate
            apm: model.player8Apm
        }

        Rectangle {
            id: rectangle
            width: 200
            height: 200
            color: "#00000000"
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}


