import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    property var itemModel
    id: itemRectangle
    width: 280 * _uiBackend.sizeModifer
    height: 130 * _uiBackend.sizeModifer
    color: (itemRectangle.itemModel.playerIsBanned) ? "#ffa9a9" : "#ffffff"
    radius: 10 * _uiBackend.sizeModifer
    Layout.maximumHeight: 100 * _uiBackend.sizeModifer
    Layout.maximumWidth: 200 * _uiBackend.sizeModifer
    Layout.fillWidth: false
    Layout.fillHeight: true

    property url avatarSource

    RowLayout {
        id: rowLayout
        anchors.fill: parent

        ColumnLayout {
            id: columnLayout
            //width: 100 * _uiBackend.sizeModifer
            //height: 100 * _uiBackend.sizeModifer

            Rectangle {
                id: rectangle
                //width: 200 * _uiBackend.sizeModifer
               // width: 0
                //height: 200 * _uiBackend.sizeModifer
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
                //font.pointSize: 10
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
                //font.pointSize: 12

                font.pixelSize: 20 * _uiBackend.sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: "MMR: " + itemRectangle.itemModel.playerMmr
                //font.pointSize: 8

                font.pixelSize: 12 * _uiBackend.sizeModifer

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: "Solo MMR: " + itemRectangle.itemModel.playerMmr1v1
                Layout.fillHeight: true
                Layout.fillWidth: true
                //font.pointSize: 8
                font.pixelSize: 12 * _uiBackend.sizeModifer
            }

            Label {
                id: label4
                text: "Games played: " + itemRectangle.itemModel.playerGamesCount
                //font.pointSize: 8
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: "Race: " + itemRectangle.itemModel.playerRace
                Layout.fillHeight: true
                Layout.fillWidth: true
               // font.pointSize: 8
                font.pixelSize: 12 * _uiBackend.sizeModifer
            }

            Label {
                id: label5
                text: "Win rate: " + itemRectangle.itemModel.playerWinRate + "%"
                //font.pointSize: 8
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: "APM: " + itemRectangle.itemModel.playerApm
                //font.pointSize: 8
                font.pixelSize: 12 * _uiBackend.sizeModifer
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

    }
}
