import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle{
    width: 600 * _uiBackend.sizeModifer
    height: textMessage.height  * _uiBackend.sizeModifer + 60
    color: "#535353"

    radius: 10 * _uiBackend.sizeModifer
    z: 100

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
                source: "qrc:/images/resources/images/warning.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        Text{

            id: textMessage
            color: "#ffffff"


            Layout.fillWidth: true

          //  text:"Alert, РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!!"
            text:"Alert, РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!!"
            font.pixelSize: 15 * _uiBackend.sizeModifer
            wrapMode: Text.WordWrap
        }

        Rectangle
        {
            id: closeRectangle

            width: 30 * _uiBackend.sizeModifer
            height: 30 * _uiBackend.sizeModifer
            //radius: 5 * _uiBackend.sizeModifer

            Layout.preferredHeight: 30 * _uiBackend.sizeModifer
            Layout.preferredWidth: 30 * _uiBackend.sizeModifer


            Layout.topMargin: 10 * _uiBackend.sizeModifer
            Layout.leftMargin: 10 * _uiBackend.sizeModifer
            Layout.rightMargin: 10 * _uiBackend.sizeModifer

            color: "#00000000"

            Layout.alignment: Qt.AlignTop

            Image{
                cache: false
                x: 0
                y: 0
                width: 30 * _uiBackend.sizeModifer
                height: 30 * _uiBackend.sizeModifer

                source: "qrc:/images/resources/images/closeButton.png"
                fillMode: Image.PreserveAspectFit
            }
        }
    }
}
