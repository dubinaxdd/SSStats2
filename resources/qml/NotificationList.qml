import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import DowStatsStyle 1.0

ColumnLayout
{
    id: root

    Item {
        Layout.preferredHeight: parent.height - listView.height
    }

    ListView{
        id: listView
        model: _uiBackend.notificationManager
        verticalLayoutDirection: ListView.BottomToTop
        clip: true
        width: 300
        Layout.preferredHeight: contentHeight > root.height ? root.height : contentHeight

        spacing: 5

        delegate: Rectangle {

            id: delegate
            color: DowStatsStyle.selectionColor
            radius: 10
            width: root.width
            height: textArea.height > 40 ? textArea.height + 20 : 60

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                clip: true

                Image {
                    id: image
                    Layout.preferredWidth: 30
                    Layout.preferredHeight: 30
                    source: model.type === 0 ? "qrc:/images/resources/images/ready.svg" : "qrc:/images/resources/images/warning.svg"
                    sourceSize.width: 30
                    sourceSize.height: 30
                }

                TextArea{
                    id: textArea
                    width: parent.width - 50
                    Layout.alignment: Qt.AlignVCenter
                    text: model.text
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                    wrapMode: Text.Wrap
                }
            }

            MouseArea{
                anchors.fill: parent

                onClicked: {
                    listView.model.removeItem(model.index);
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }

        MouseArea{
            anchors.fill: parent
            propagateComposedEvents: true
        }
    }
}
