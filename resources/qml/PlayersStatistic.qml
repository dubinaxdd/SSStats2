import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: mainRectangle

    width: 260 * _uiBackend.sizeModifer

    Layout.minimumWidth: 260 * _uiBackend.sizeModifer
    Layout.maximumWidth: 260 * _uiBackend.sizeModifer

    color: "#00000000"
    border.color: "#00000000"

    property var model
    property Rectangle expandPatyStatisticButtonRectangle : expandPatyStatisticButtonRectangle

    property int relativeMouseX
    property int relativeMouseY

    function mouseWheel(delta)
    {
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


    function mouseHover(x, y)
    {
        relativeMouseX = x
        relativeMouseY = y
    }

    function visibleItemsCount()
    {
        var asd = 1;

        if (player2.visible)
            asd++;

        if (player3.visible)
            asd++;

        if (player4.visible)
            asd++;

        if (player5.visible)
            asd++;

        if (player6.visible)
            asd++;

        if (player7.visible)
            asd++;

        if (player8.visible)
            asd++;

        return asd;
    }

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
        //anchors.fill: parent

        height: (120 + columnLayout.spacing) * visibleItemsCount() *_uiBackend.sizeModifer

        width: 260 *_uiBackend.sizeModifer

        function scrollToBottom() {

            if (ScrollBar.vertical.position + 0.125  < 0.9 )
                ScrollBar.vertical.position += 0.125
        }

        function scrollToTop() {

            if (ScrollBar.vertical.position - 0.125  >= 0 )
                ScrollBar.vertical.position -= 0.125
        }

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent


            PlayersStatisticItem
            {
                id:curentPlayer
                visible: model.curentPlayerStatsItem.itemVisible && (!model.expandPatyStatistic || _uiBackend.expand) && model.curentPlayerStatsItem.playerName !== ""

                itemModel: model.curentPlayerStatsItem
                avatarSource: "image://ImageProvider/currentPlayerAvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player2
                //visible: model.player2StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player2StatsItem.playerName !== ""
                visible: true
                itemModel: model.player2StatsItem
                avatarSource: "image://ImageProvider/player2AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player3
                //visible: model.player3StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player3StatsItem.playerName !== ""
                visible: true
                itemModel: model.player3StatsItem
                avatarSource: "image://ImageProvider/player3AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player4
                //visible: model.player4StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player4StatsItem.playerName !== ""
                visible: true
                itemModel: model.player4StatsItem
                avatarSource: "image://ImageProvider/player4AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player5
                //visible: model.player5StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player5StatsItem.playerName !== ""
                visible: true
                itemModel: model.player5StatsItem
                avatarSource: "image://ImageProvider/player5AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player6
                //visible: model.player6StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player6StatsItem.playerName !== ""
                visible: true
                itemModel: model.player6StatsItem
                avatarSource: "image://ImageProvider/player6AvatarMedium"

            }

            PlayersStatisticItem
            {
                id:player7
                //visible: model.player7StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player7StatsItem.playerName !== ""
                visible: true
                itemModel: model.player7StatsItem
                avatarSource: "image://ImageProvider/player7AvatarMedium"

            }


            PlayersStatisticItem
            {
                id:player8
                //visible: model.player8StatsItem.itemVisible && (!model.expandPatyStatistic  || _uiBackend.expand) && model.player8StatsItem.playerName !== ""
                visible: true

                itemModel: model.player8StatsItem
                avatarSource: "image://ImageProvider/player8AvatarMedium"

            }


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
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}


