import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1

Rectangle {

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 20

        RowLayout
        {
            Layout.preferredHeight: miniMapRectangle.height
            Layout.fillWidth: true

            spacing: 20

            Rectangle
            {
                id: miniMapRectangle

                Layout.preferredWidth: 140
                Layout.preferredHeight: 140

                color: "#112332"
                radius: 10
                clip: true

                Image{
                    id: mapImage
                    anchors.fill: parent
                    //source: //_uiBackend.replayManager.mapSourceUrl
                    cache: false
                    visible:false
                    fillMode: Image.PreserveAspectFit
                }

                Rectangle {
                    id: maskRectangle

                    anchors.fill: parent
                    radius: 10
                    visible: false

                }

                Label
                {
                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter

                    anchors.fill: parent
                    color: "#FFFFFF"
                    text: "You need to\ndownload the map\nto show the image."
                }

                OpacityMask {
                    id: opacityMask
                    anchors.fill: mapImage
                    source: mapImage
                    maskSource: maskRectangle
                }

            }

            ColumnLayout
            {
                Layout.alignment: Qt.AlignTop

                Label{
                    text: _uiBackend.mapManagerPage.currentMapName
                    font.pixelSize: 20
                }

                Label{
                    text: "Authors: " + _uiBackend.mapManagerPage.currentMapAuthors
                }

                Label{
                    text: _uiBackend.mapManagerPage.currentMapDescription
                }

                Label{
                    text: "Tags: " + _uiBackend.mapManagerPage.currentMapTags
                }
            }
        }

        ListView {
            id: mapsListView

            Layout.fillHeight: true
            Layout.fillWidth: true

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
                color: model.selected ? "#A9A9A9" : delegateMouseArea.containsMouse ? "#c8c8c8" : "#DCDCDC"
                //color: "#DCDCDC"

                MouseArea{
                    id: delegateMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    z:0

                    onClicked: {
                        _uiBackend.mapManagerPage.selectMap(model.index)
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
                        //visible: model.description !== ""
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
}
