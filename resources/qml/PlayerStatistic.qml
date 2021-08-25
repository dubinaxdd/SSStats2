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

    //property var model
    property url avatarSource
    property string name
    property string mmr
    property string mmr1v1
    property string gamesCount
    property string race
    property string winRate
    property string apm


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

                source: rectangle1.avatarSource
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
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
                text: rectangle1.name
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: "MMR: " + rectangle1.mmr
                font.pointSize: 8
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: "Solo MMR: " + rectangle1.mmr1v1
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label4
                text: "Games played: " + rectangle1.gamesCount
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: "Race: " + rectangle1.race
                Layout.fillHeight: true
                Layout.fillWidth: true
                font.pointSize: 8
            }

            Label {
                id: label5
                text: "Win rate: " + rectangle1.winRate + "%"
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: "APM: " + rectangle1.apm
                font.pointSize: 8
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
