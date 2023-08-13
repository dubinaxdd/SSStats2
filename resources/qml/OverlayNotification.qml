import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle{
    id: mainRectangle

    width: 600 * _uiBackend.sizeModifer
    height: textMessage.height  * _uiBackend.sizeModifer + 60
    color: DowStatsStyle.backgroundColor
    visible: _uiBackend.notificationVisible

    radius: 10 * _uiBackend.sizeModifer
    z: 100

    property int relativeMouseX
    property int relativeMouseY

    property bool isWarning: _uiBackend.lastNotificationIsWarning

    RowLayout{

        id: mainRowLayout

        anchors.fill: parent

        Rectangle{
            id: iconRectangle

            width: 40 * _uiBackend.sizeModifer
            height: 40 * _uiBackend.sizeModifer
            Layout.preferredHeight: 40 * _uiBackend.sizeModifer
            Layout.preferredWidth: 40 * _uiBackend.sizeModifer
            Layout.leftMargin: 10 * _uiBackend.sizeModifer
            Layout.rightMargin: 10 * _uiBackend.sizeModifer

            color: "#00000000"

            Image {
                id: image
                anchors.fill: parent
                source: isWarning ? "qrc:/images/resources/images/warning.svg" : "qrc:/images/resources/images/ready.svg"
                sourceSize.width: width
                sourceSize.height: height
            }
        }

        Text{

            id: textMessage
            color: DowStatsStyle.textColor

            Layout.fillWidth: true
            text: _uiBackend.lastNotification
            font.pixelSize: 15 * _uiBackend.sizeModifer
            wrapMode: Text.WordWrap
        }

        Rectangle
        {
            id: closeRectangle

            width: 40 * _uiBackend.sizeModifer
            height: 40 * _uiBackend.sizeModifer
            radius: 5 * _uiBackend.sizeModifer

            Layout.preferredHeight: 30 * _uiBackend.sizeModifer
            Layout.preferredWidth: 30 * _uiBackend.sizeModifer


            Layout.topMargin: 10 * _uiBackend.sizeModifer
            Layout.leftMargin: 10 * _uiBackend.sizeModifer
            Layout.rightMargin: 10 * _uiBackend.sizeModifer

            color: "#00000000"

            Layout.alignment: Qt.AlignTop |Qt.AlignHCenter

            Image {
                anchors.fill: parent
                anchors.margins: 5
                source: "qrc:/images/resources/images/close.svg"
                sourceSize.width: width
                sourceSize.height: height
            }

            GlobalMouseArea{
                id: closeRectangleButton
                anchors.fill: parent

                onClicked: {
                    _uiBackend.notificationVisible = false
                }

                onHoveredChanged: {
                    if (hovered)
                        closeRectangle.color = "#FFFFFF"
                    else
                        closeRectangle.color = "#00000000"
                }
            }
        }
    }
}
