import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: fullOverlayRectangle
    width: 600
    height: 600
    opacity: 1
    color: "#00000000"
    border.color: "#00000000"
    transformOrigin: Item.BottomRight
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    Layout.fillHeight: true

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        Rectangle {
            id: fullOverlayHeader
            width: 260
            height: 60
            Layout.fillWidth: true
            Layout.minimumWidth: 260
            Layout.minimumHeight: 60
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#333333"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            Layout.maximumHeight: 60
            Layout.maximumWidth: 26099
            Layout.fillHeight: false
        }

        Rectangle {
            id: rectangle
            width: 200
            height: 200
            opacity: 1
            color: "#ffffff"
            Layout.margins: 60
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
