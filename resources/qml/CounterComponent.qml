import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Item
{
    id: root
    property int count: 0
    property string text: ""

    Layout.preferredHeight: label.height
    Layout.fillWidth: true

    RowLayout{
        anchors.fill: parent

        Rectangle
        {
            Layout.topMargin: 2

            Layout.preferredHeight: 10
            Layout.preferredWidth: 10

            Layout.alignment: Qt.AlignVCenter

            color:"#00000000"

            Image {
               anchors.fill: parent
               source: "qrc:/images/resources/images/man_user.svg"
               sourceSize.width: 10
               sourceSize.height: 10
            }
        }

        Label{
            id: label
            text: root.text
            Layout.preferredWidth: 200
            clip: true
            font.pixelSize: 11
            color: DowStatsStyle.textColor
        }

        Rectangle{
            Layout.fillWidth: true
        }

        Label{
            text: root.count
            font.pixelSize: 11
            color: DowStatsStyle.textColor
        }
    }
}
