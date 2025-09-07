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

    function resetScrollView()
    {
        scrollView.setDefault();
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
    }

    Connections
    {
        target: _uiBackend


        function onSendExpand()
        {
            scrollViewPosition = 0.0;
        }
    }

    ScrollView {
        id: scrollView

        height: mainRectangle.height
        width: mainRectangle.width

        GlobalMouseArea
        {
            id: scrollViewMouseArea
            anchors.fill: parent

            onWheel: {
                if (delta > 0)
                    scrollView.scrollToTop();

                if (delta < 0)
                    scrollView.scrollToBottom()
            }
        }

        function scrollToBottom() {

            var cof = 0.04 * _uiBackend.sizeModifer


            if (ScrollBar.vertical.position + cof <= 0.9 && (!model.expandPatyStatistic || _uiBackend.expand) && (expandPatyStatisticButtonRectangle.y - (scrollView.height * (scrollViewPosition + cof))) >= 0.0 )
                ScrollBar.vertical.position += cof

            scrollViewPosition = ScrollBar.vertical.position
        }

        function scrollToTop() {

            var cof = 0.04 * _uiBackend.sizeModifer

            if ((!model.expandPatyStatistic || _uiBackend.expand) )
                ScrollBar.vertical.position -= cof

            if (ScrollBar.vertical.position < 0)
                ScrollBar.vertical.position = 0

            scrollViewPosition = ScrollBar.vertical.position
        }

        function setDefault()
        {
            ScrollBar.vertical.position = 0.0
            scrollViewPosition = 0.0
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
                playerRank: model.curentPlayerStatsItem.playerRank
                playerRace: model.curentPlayerStatsItem.playerRace
                playerWinRate: model.curentPlayerStatsItem.playerWinRate
                playerApm: model.curentPlayerStatsItem.playerApm
                playerIsBanned: model.curentPlayerStatsItem.playerIsBanned
                playerVisible: model.curentPlayerStatsItem.itemVisible
                banType: model.curentPlayerStatsItem.banType
                //calibrateGamesLeft: model.curentPlayerStatsItem.calibrateGamesLeft
                isRanked: model.curentPlayerStatsItem.isRanked
                isOnline: model.curentPlayerStatsItem.isOnline

                avatarSource: "image://imageprovider/currentPlayerAvatarMedium"
                sizeModifer: _uiBackend.sizeModifer
                hoverEnabled: false

            }

            ListView
            {
                id: playersListView

                model:_uiBackend.statisticPanel
                visible: (!model.expandPatyStatistic || _uiBackend.expand)

                Layout.preferredHeight: ((120 + 5) * count  - 5) * _uiBackend.sizeModifer
                Layout.maximumHeight: ((120 + 5) * count - 5) * _uiBackend.sizeModifer
                Layout.minimumHeight: ((120 + 5) * count - 5) * _uiBackend.sizeModifer

                spacing: 5 * _uiBackend.sizeModifer

                delegate: PlayersStatisticItem{

                    playerName: model.playerName
                    playerMmr: model.playerMmr
                    playerMmr1v1: model.playerMmr1v1
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

                    avatarSource: "image://imageprovider/" + model.avatarId
                    sizeModifer: _uiBackend.sizeModifer
                    hoverEnabled: false

                    height: 120 * _uiBackend.sizeModifer
                    width: 280 * _uiBackend.sizeModifer
                }
            }

            Rectangle {
                id: expandPatyStatisticButtonRectangle

                property bool howeredState: expandButton.hovered
                visible: _uiBackend.expandStatisticButtonVisible

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

                Layout.maximumWidth: 280 * _uiBackend.sizeModifer
                Layout.minimumWidth: 280 * _uiBackend.sizeModifer
                width: 280 * _uiBackend.sizeModifer

                Layout.maximumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                Layout.minimumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                height: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0

                radius: 5 * _uiBackend.sizeModifer

                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                //visible: !_uiBackend.expand
                gradient: howeredState ? grDark : grLight

                Image {
                    id: image

                    visible: !_uiBackend.expand
                    width: 95 * _uiBackend.sizeModifer
                    height: 10 * _uiBackend.sizeModifer
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/images/resources/images/expandDots.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                }

                GlobalMouseArea{
                    id: expandButton
                    anchors.fill: parent

                    onClicked: {
                        _uiBackend.expandPatyStatisticButtonClick();
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color:"#00000000"
            }
        }
    }
}


