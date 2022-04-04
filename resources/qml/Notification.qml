import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle{
    width: mainRowLayout.width
    height: mainRowLayout.height
    //width: /*450 * _uiBackend.sizeModifer*/ textMessage.width + closeRectangle.width + iconRectangle.width + (20 * 4 * _uiBackend.sizeModifer)
    //height: (textMessage.height + 60) * _uiBackend.sizeModifer
    color: "#535353"

    radius: 10 * _uiBackend.sizeModifer
    z: 100

    RowLayout{

        id: mainRowLayout
        x:0
        y:0
        //anchors.fill: parent

        Rectangle{
            id: iconRectangle

            width: 40 * _uiBackend.sizeModifer
            height: 40 * _uiBackend.sizeModifer

            Layout.leftMargin: 20 * _uiBackend.sizeModifer
            Layout.rightMargin: 20 * _uiBackend.sizeModifer

            color: "#00000000"

            Image{
                cache: false
                width: 40 * _uiBackend.sizeModifer
                height: 40 * _uiBackend.sizeModifer
                x: 0
                y: 0
                //anchors.fill: parent
                source: "qrc:/images/resources/images/warning.png"
                fillMode: Image.PreserveAspectFit
            }
        }

        Text{

            id: textMessage
            color: "#ffffff"
            width: 300 * _uiBackend.sizeModifer
            Layout.maximumWidth: 300 * _uiBackend.sizeModifer
            text:"Alert, РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!! РЕПЛЕЙ НЕ ОТПРАВЛЕН БЛЕАТЬ!!!"

            font.pixelSize: 15 * _uiBackend.sizeModifer
            wrapMode: Text.WordWrap
        }


        Rectangle{
            Layout.fillWidth: true
        }


        Rectangle
        {
            id: closeRectangle

            width: 30 * _uiBackend.sizeModifer
            height: 30 * _uiBackend.sizeModifer
            radius: 5 * _uiBackend.sizeModifer

            Layout.leftMargin: 20 * _uiBackend.sizeModifer
            Layout.rightMargin: 20 * _uiBackend.sizeModifer

            color: "#00000000"

            //Layout.alignment: Qt.AlignTop

            Image{
                cache: false
                x: 0
                y: 0
                width: 30 * _uiBackend.sizeModifer
                height: 30 * _uiBackend.sizeModifer

                //anchors.fill: parent
                source: "qrc:/images/resources/images/closeButton.png"
                fillMode: Image.PreserveAspectFit
            }
        }
    }
}
