import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    id: rectangle1
    width: 250
    height: 120
    color: "#ffffff"
    radius: 10
    Layout.maximumHeight: 100
    Layout.maximumWidth: 200
    Layout.fillWidth: false
    Layout.fillHeight: true
    //anchors.horizontalCenter: parent.horizontalCenter

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
                id: image
                x: 137
                y: 0
                width: 60
                height: 60
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/qtquickplugin/images/template_image.png"
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
            }
        }

        ColumnLayout {
            id: columnLayout4
            Layout.margins: 5
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.bottomMargin: 0

            Label {
                id: label1
                text: qsTr("TranspersonalPsy")
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label2
                text: qsTr("MMR: 100500")
                font.pointSize: 6
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: label
                text: qsTr("Solo MMR: 200500")
                font.pointSize: 6
            }

            Label {
                id: label4
                text: qsTr("Games Played 250000")
                font.pointSize: 6
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label3
                text: qsTr("Ladder position: 1")
                font.pointSize: 6
            }

            Label {
                id: label5
                text: qsTr("Win rate: 100%")
                font.pointSize: 6
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: label6
                text: qsTr("APM: 450")
                font.pointSize: 6
                Layout.fillHeight: true
                Layout.fillWidth: true
            }


        }


    }

}
