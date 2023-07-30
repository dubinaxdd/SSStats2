import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle{
    id: mainRectangle

    width: 600 * _uiBackend.sizeModifer
    height: textMessage.height  * _uiBackend.sizeModifer + 60
    color: "#535353"
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

            Image{
                cache: false
                width: 40 * _uiBackend.sizeModifer
                height: 40 * _uiBackend.sizeModifer
                x: 0
                y: 0
                source: isWarning ? "qrc:/images/resources/images/warning.png" : "qrc:/images/resources/images/readyImage.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        Text{

            id: textMessage
            color: "#ffffff"

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

            Image{
                cache: false
                x: 0
                y: 0
                width: 30 * _uiBackend.sizeModifer
                height: 30 * _uiBackend.sizeModifer

                source: "qrc:/images/resources/images/closeButton.png"
                fillMode: Image.PreserveAspectFit
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
