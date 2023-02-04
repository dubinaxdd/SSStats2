import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {

    ListView {
        id: mapsListView
        anchors.fill: parent
        clip: true
        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            active: true
            onActiveChanged: {
                        active = true
            }
        }

        model: _uiBackend.mapManagerPage

        spacing: 5

        delegate:  Rectangle {

            width: mapsListView.width - scrollBar.width
            height: 50
            radius: 10
            //color: model.selected ? "#A9A9A9" : delegateMouseArea.containsMouse ? "#DCDCDC" : "#DCDCDC"
            color: "#DCDCDC"

            MouseArea{
                id: delegateMouseArea
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                hoverEnabled: true
                z:0

                onClicked: {
                    //_uiBackend.replayManager.replaysListModel.setSelected(model.index);
                }
            }

            RowLayout
            {
                anchors.fill: parent

                anchors.margins: 10

                spacing: 10

                Rectangle{

                    Layout.preferredWidth: 10
                    Layout.preferredHeight: 10
                    anchors.margins: 5

                    color: "#00000000"

                    Image {
                        visible: model.needInstall || model.needUpdate
                        anchors.fill: parent
                        source: "qrc:/images/resources/images/redDot.png"
                    }
                }

                Label
                {
                    Layout.preferredWidth: 200
                    text: model.mapName
                }

                Label
                {
                    Layout.preferredWidth: 60
                    text: model.description
                    visible: model.description !== ""
                }

                Label
                {
                    Layout.preferredWidth: 100
                    text: "Tags: " + model.tags
                    visible: model.tags !== ""
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }

                Label
                {
                    text: "Downloading processed..."
                    visible: model.downloadingProcessed
                }

                BlueButton{
                    text: "Install"
                    z:2
                    visible: model.needInstall && !model.downloadingProcessed

                    onClicked: _uiBackend.mapManagerPage.installMap(model.index)
                }

                BlueButton{
                    text: "Update"
                    z:2
                    visible: !model.needInstall && model.needUpdate && !model.downloadingProcessed

                    onClicked: _uiBackend.mapManagerPage.installMap(model.index)
                }

                BlueButton{
                    text: "Delete"
                    z:2
                    visible: !model.needInstall && !model.needUpdate && !model.downloadingProcessed

                    onClicked: _uiBackend.mapManagerPage.removeMap(model.index);
                }
            }
        }
    }
}
