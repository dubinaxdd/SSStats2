import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    id: rectangle1
    width: 280
    height: 130
    color: "#ffffff"
    radius: 10
    Layout.maximumHeight: 100
    Layout.maximumWidth: 200
    Layout.fillWidth: false
    Layout.fillHeight: true

    property var model

    RowLayout {
        id: rowLayout
        anchors.fill: parent

        Rectangle {
            id: rectangle
            width: 200
            height: 200
            color: "#ffffff"
            radius: 10
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.minimumHeight: 60
            Layout.minimumWidth: 60
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

                source: "image://ImageProvider/currentPlayerAvatarMedium"
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit

                //Костыль для перезагрузки картинки, рил так на формух делают
                function reload() {
                    var oldSource = avatarImage.source;
                    avatarImage.source = "";
                    avatarImage.source = oldSource;
                }

                Connections {
                    target: model

                    function onCurrentPlayerStatsChanged()
                    {
                        avatarImage.reload();
                    }
                }
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
                text:model.currentPlayerName
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: "MMR: " + model.currentPlayerMmr
                font.pointSize: 8
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: "Solo MMR: " + model.currentPlayerMmr1v1
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label4
                text: "Games played: " + model.currentPlayerGamesCount
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: "Race: " + model.currentPlayerRace
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label5
                text: "Win rate: " + model.currentPlayerWinRate + "%"
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: "APM: " + model.currentPlayerApm
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }


        }


    }

}
