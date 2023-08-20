import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle{
    id: root

    anchors.fill: parent
    color: "#cf1a1919"
    z: 10

    property bool yesButtonVisible: true
    property bool noButtonVisible: true
    property bool okButtonVisible: true

    property string text: ""

    signal clickedYes()
    signal clickedNo()
    signal clickedOk()

    Rectangle
    {
        id: messageRectangle
        color: DowStatsStyle.backgroundColor
        radius: 10
        width:400
        height: textArea.height > 40 ? textArea.height + 50 : 90
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter

        ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 10

            RowLayout
            {
                Layout.alignment: Qt.AlignHCenter

                Image {
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    source: "qrc:/images/resources/images/warning.svg"
                    sourceSize.width: 30
                    sourceSize.height: 30
                }

                TextArea{
                    id: textArea
                    width: messageRectangle.width - 50
                    Layout.alignment: Qt.AlignVCenter
                    text: root.text
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                    wrapMode: Text.Wrap
                }
            }

            RowLayout
            {
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 15

                BlueButton
                {
                    visible: root.yesButtonVisible
                    text: "Yes"
                    onClicked: root.clickedYes();
                }

                BlueButton
                {
                    visible: root.noButtonVisible
                    text: "No"
                    onClicked: root.clickedNo();
                }

                BlueButton
                {
                    visible: root.okButtonVisible
                    text: "Ok"
                    onClicked: root.clickedOk();
                }
            }
        }
    }
}
