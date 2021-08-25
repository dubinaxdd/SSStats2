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

    function reload() {
        var oldSource = curentPlayer.avatarSource;
        curentPlayer.avatarSource = "";
        curentPlayer.avatarSource = oldSource;
    }

    Connections {
        target: model

        function onCurrentPlayerStatsChanged()
        {
            reload();
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
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }

        PlayerStatistic
        {
            id:player3
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }

        PlayerStatistic
        {
            id:player4
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }

        PlayerStatistic
        {
            id:player5
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }

        PlayerStatistic
        {
            id:player6
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }

        PlayerStatistic
        {
            id:player7
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
        }


        PlayerStatistic
        {
            id:player8
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
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


