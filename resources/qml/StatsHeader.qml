import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12


Rectangle {
    id: headerRectangle
    width: 260
    height: 60
    color: "#333333"
    Layout.minimumWidth: 260
    Layout.minimumHeight: 60

    property Rectangle expandButtonRectangle : expandButtonRectangle

    //function getObj() { return obj }

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
    Layout.maximumWidth: 260
    Layout.fillHeight: false

    RowLayout {
        id: rowLayout2
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: rectangle5
            width: 200
            height: 200
            color: "#00000000"
            border.color: "#00000000"
            Layout.maximumHeight: 60
            Layout.maximumWidth: 60
            Layout.minimumHeight: 60
            Layout.minimumWidth: 60
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: image
                anchors.fill: parent
                source: "qrc:/images/resources/images/ssstats.png"
                anchors.rightMargin: 10
                anchors.leftMargin: 10
                anchors.bottomMargin: 10
                anchors.topMargin: 10
                Layout.minimumHeight: 60
                Layout.minimumWidth: 60
                Layout.margins: 0
                Layout.rightMargin: 0
                Layout.leftMargin: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                fillMode: Image.PreserveAspectFit
            }
        }

        ColumnLayout {
            id: columnLayout2
            width: 100
            height: 100
            Layout.fillWidth: false
            Layout.fillHeight: false

            Label {
                id: label
                color: "#ffffff"
                text: qsTr("SS Stats 2")
                font.pointSize: 20
            }

            Label {
                id: versionLabel
                color: "#999999"
                text: "v" + _uiBackend.ssStatsVersion
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }

        Rectangle {
            id: expandButtonRectangle

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

            width: 200
            height: 200
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.columnSpan: 0
            Layout.rowSpan: 0
            Layout.maximumHeight: 60
            Layout.maximumWidth: 60
            Layout.minimumHeight: 60
            Layout.minimumWidth: 60
            Layout.fillHeight: true
            Layout.fillWidth: true

            gradient: howeredState ? grDark : grLight

            BorderImage {
                id: borderImage
                anchors.fill: parent
                source: _uiBackend.expand ? "qrc:/images/resources/images/hide.png" : "qrc:/images/resources/images/expand.png"
            }
        }
    }
}



