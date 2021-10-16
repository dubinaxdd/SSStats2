import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    property var itemModel
    id: itemRectangle
    width: 280
    height: 130
    color: (itemRectangle.itemModel.playerIsBanned) ? "#ffa9a9" : "#ffffff"
    radius: 10
    Layout.maximumHeight: 100
    Layout.maximumWidth: 200
    Layout.fillWidth: false
    Layout.fillHeight: true

    property url avatarSource

    RowLayout {
        id: rowLayout
        anchors.fill: parent

        ColumnLayout {
            id: columnLayout
            width: 100
            height: 100

            Rectangle {
                id: rectangle
                width: 200
                height: 200
                color: "#00000000"
                radius: 10
                Layout.topMargin: itemRectangle.itemModel.playerIsBanned ? 10 : 0
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.maximumWidth: 80
                Layout.preferredHeight: 60
                Layout.preferredWidth: 80
                Layout.minimumHeight: 60
                Layout.minimumWidth: 80
                Layout.fillHeight: true
                Layout.fillWidth: true

                Image {
                    id: avatarImage
                    cache: false
                    x: 137
                    y: 0
                    width: 60
                    height: 60
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
                font.pointSize: 10
                Layout.bottomMargin: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout {
            id: columnLayout4
            spacing: 0
            Layout.margins: 5
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.bottomMargin: 0

            Label {
                id: label1
                text: itemRectangle.itemModel.playerName
                Layout.maximumWidth: 180
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: "MMR: " + itemRectangle.itemModel.playerMmr
                font.pointSize: 8
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: "Solo MMR: " + itemRectangle.itemModel.playerMmr1v1
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label4
                text: "Games played: " + itemRectangle.itemModel.playerGamesCount
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: "Race: " + itemRectangle.itemModel.playerRace
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label5
                text: "Win rate: " + itemRectangle.itemModel.playerWinRate + "%"
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: "APM: " + itemRectangle.itemModel.playerApm
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

    }
}
