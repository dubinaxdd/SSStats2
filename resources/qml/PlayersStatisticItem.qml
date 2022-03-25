import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    property var itemModel
    id: itemRectangle

    Layout.maximumWidth: 280 * _uiBackend.sizeModifer
    Layout.maximumHeight: 120 * _uiBackend.sizeModifer
    Layout.minimumWidth: 280 * _uiBackend.sizeModifer
    Layout.minimumHeight: 120 * _uiBackend.sizeModifer
    height: 120 * _uiBackend.sizeModifer
    Layout.alignment: Qt.AlignRight | Qt.AlignTop
    Layout.fillHeight: false
    Layout.fillWidth: true

    color: (itemRectangle.itemModel.playerIsBanned) ? "#ffa9a9" : "#ffffff"
    radius: 10 * _uiBackend.sizeModifer
    property url avatarSource

    RowLayout {
        id: rowLayout
        anchors.fill: parent

        ColumnLayout {
            id: columnLayout

            Rectangle {
                id: rectangle

                color: "#00000000"
                radius: 10 * _uiBackend.sizeModifer
                Layout.topMargin: (itemRectangle.itemModel.playerIsBanned ? 10 : 0) * _uiBackend.sizeModifer
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.maximumWidth: 80 * _uiBackend.sizeModifer
                Layout.preferredHeight: 60 * _uiBackend.sizeModifer
                Layout.preferredWidth: 80 * _uiBackend.sizeModifer
                Layout.minimumHeight: 60 * _uiBackend.sizeModifer
                Layout.minimumWidth: 80 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true

                Image {
                    id: avatarImage
                    cache: false
                    x: 137 * _uiBackend.sizeModifer
                    y: 0 * _uiBackend.sizeModifer
                    width: 60 * _uiBackend.sizeModifer
                    height: 60 * _uiBackend.sizeModifer
                    anchors.verticalCenter: parent.verticalCenter

                    source: itemRectangle.avatarSource
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                }

            }

            Label {
                id: statusLabel
                visible: itemRectangle.itemModel.playerIsBanned
                text: qsTr("[CHEATER]")
                font.pixelSize: 15 * _uiBackend.sizeModifer

                Layout.bottomMargin: 10 * _uiBackend.sizeModifer
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout {
            id: columnLayout4
            spacing: 0
            Layout.margins: 5 * _uiBackend.sizeModifer
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.bottomMargin: 0

            Label {
                id: label1
                text: itemRectangle.itemModel.playerName
                Layout.maximumWidth: 180 * _uiBackend.sizeModifer

                font.pixelSize: 20 * _uiBackend.sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: "MMR: " + itemRectangle.itemModel.playerMmr
                font.pixelSize: 12 * _uiBackend.sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: "Solo MMR: " + itemRectangle.itemModel.playerMmr1v1
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pixelSize: 12 * _uiBackend.sizeModifer
            }

            Label {
                id: label4
                text: "Games played: " + itemRectangle.itemModel.playerGamesCount
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: "Race: " + itemRectangle.itemModel.playerRace
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pixelSize: 12 * _uiBackend.sizeModifer
            }

            Label {
                id: label5
                text: "Win rate: " + itemRectangle.itemModel.playerWinRate + "%"
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: "APM: " + itemRectangle.itemModel.playerApm
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

    }
}
