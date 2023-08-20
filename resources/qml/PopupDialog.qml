import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle{
    id: root

    anchors.fill: parent
    color: "#cf1a1919"
    z: 10
    visible: _uiBackend.balanceModPage.profileCopyModeRequestMessageVisible

    property string text: ""
    signal clickedYes();
    signal clickedNo();

    Rectangle
    {
        color: DowStatsStyle.backgroundColor
        radius: 10
        width:400
        height: 150
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter

        ColumnLayout
        {
            anchors.fill: parent

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
                    width: 350
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
                    text: "Yes"
                    onClicked: root.clickedYes();
                }

                BlueButton
                {
                    text: "No"
                    onClicked: root.clickedNo();
                }
            }
        }
    }
}
