import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    width: 260
    height: 880
    color: "#00000000"
    border.color: "#00000000"

    property var model
    property Rectangle expandPatyStatisticButtonRectangle : expandPatyStatisticButtonRectangle

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

        PlayersStatisticItem
        {
            id:curentPlayer
            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true
            visible: model.curentPlayerStatsItem.itemVisible && (!model.expandPatyStatistic || _uiBackend.expand) && model.curentPlayerStatsItem.playerName !== ""

            itemModel: model.curentPlayerStatsItem
            avatarSource: "image://ImageProvider/currentPlayerAvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player2
            visible: model.player2StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player2StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player2StatsItem
            avatarSource: "image://ImageProvider/player2AvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player3
            visible: model.player3StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player3StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player3StatsItem
            avatarSource: "image://ImageProvider/player3AvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player4
            visible: model.player4StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player4StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player4StatsItem
            avatarSource: "image://ImageProvider/player4AvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player5
            visible: model.player5StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player5StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player5StatsItem
            avatarSource: "image://ImageProvider/player5AvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player6
            visible: model.player6StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player6StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player6StatsItem
            avatarSource: "image://ImageProvider/player6AvatarMedium"

        }

        PlayersStatisticItem
        {
            id:player7
            visible: model.player7StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player7StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player7StatsItem
            avatarSource: "image://ImageProvider/player7AvatarMedium"

        }


        PlayersStatisticItem
        {
            id:player8
            visible: model.player8StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player8StatsItem.playerName !== ""

            Layout.maximumWidth: 260
            Layout.maximumHeight: 100
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.fillHeight: false
            Layout.fillWidth: true

            itemModel: model.player8StatsItem
            avatarSource: "image://ImageProvider/player8AvatarMedium"

        }


        Rectangle {
            id: rectangle1
            visible: _uiBackend.ssWindowed && !_uiBackend.headerVisible
            color: "#00000000"
            Layout.fillWidth: true
            Layout.preferredHeight: 25
            Layout.maximumHeight: 25
            Layout.minimumHeight: 25
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
            height: 10

            //color: "#337ab7"
            radius: 5

            Layout.maximumHeight: 10
            Layout.minimumHeight: 10
            Layout.maximumWidth: 65535
            Layout.minimumWidth: 0
            //text: qsTr("***")
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !_uiBackend.expand


            gradient: howeredState ? grDark : grLight

            Image {
                id: image
                width: 95
                height: 10
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/images/resources/images/expandDots.png"
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
            }
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


