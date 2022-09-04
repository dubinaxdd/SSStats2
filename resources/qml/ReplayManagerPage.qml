import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15

Rectangle {
    id: infoRectangle
    opacity: 1
    color: "#ffffff"
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10


    RowLayout
    {
        anchors.fill: parent

        ColumnLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Layout.margins: 15

            RowLayout
            {

                spacing: 30

                ColumnLayout
                {
                    Rectangle
                    {
                        Layout.preferredWidth: 140
                        Layout.preferredHeight: 140

                        color: "#112332"
                        radius: 10

                        clip: true


                        Image{
                            id: mapImage
                            anchors.fill: parent
                            source: _uiBackend.replayManager.mapSourceUrl
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

                        OpacityMask {
                            id: opacityMask
                            anchors.fill: mapImage
                            source: mapImage
                            maskSource: maskRectangle
                        }

                    }

                    Label{
                        text: _uiBackend.replayManager.currentMap
                        Layout.alignment: Qt.AlignCenter
                    }

                }

                ColumnLayout
                {
                    Layout.maximumWidth: 340
                    clip: true

                    Label{
                        text: _uiBackend.replayManager.currentReplayName
                        font.pixelSize: 20

                    }

                    Label{
                        text: "File name: " + _uiBackend.replayManager.currentFileName
                    }

                    Label{
                        text: "Mod: " + _uiBackend.replayManager.currentMod
                    }

                    Label{
                        text: "Creation time: " + _uiBackend.replayManager.currentReplayTime
                    }

                    Label{
                        text: "Duration: " + _uiBackend.replayManager.currentDuration
                    }

                    Label{
                        text: "Players count: " + _uiBackend.replayManager.currentPlayerCount
                    }

                    Label{
                        text: "Teams count: " + _uiBackend.replayManager.currentTeamsCount
                    }

                    Label{
                        text: "Map size: " + _uiBackend.replayManager.currentMapSize
                    }
                }
            }

            Rectangle
            {
                Layout.fillHeight: true
            }

/*
            ListView {
                width: 180; height: 200

                model: asd


                delegate: Text {
                    text: name + ": " + number
                }
            }*/
        }

        Rectangle
        {
            Layout.fillWidth: true
        }

        ColumnLayout
        {
            Layout.fillHeight: true
            Layout.preferredWidth: 300
            Layout.margins: 10

            RowLayout
            {
                ComboBox{

                    Layout.preferredWidth: 225
                    model: ["Sort by file name", "Sort by name", "Sort by mod", "Sort by map", "Sort by time" ]

                    onCurrentIndexChanged: _uiBackend.replayManager.replaysListModel.setSortType(currentIndex);
                }

                Button
                {
                    Layout.preferredWidth: 70
                    text: "Update"

                    onClicked: _uiBackend.replayManager.update();
                }
            }

            Rectangle
            {
                id: replaysListRectangle

                Layout.preferredWidth: 300
                Layout.fillHeight: true

                color: "#eaeaea"
                radius: 10

                ListView{
                    id: replaysListView
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    spacing: 5

                    model: _uiBackend.replayManager.replaysListModel

                    delegate: Rectangle{

                        width: replaysListView.width
                        height: 70
                        radius: 10

                        color: delegateMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"

                        RowLayout
                        {

                            anchors.fill: parent
                            anchors.margins: 10
                            clip: true
                            spacing: 10

                            Layout.alignment: Qt.AlignLeft

                            Rectangle
                            {
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 40
                                color: "#112332"
                                radius: 10
                                clip: true

                                Image{
                                    id: mapImage2
                                    anchors.fill: parent
                                    source: model.map
                                    cache: false
                                    visible:false
                                    fillMode: Image.PreserveAspectFit
                                }

                                Rectangle {
                                    id: maskRectangle2

                                    anchors.fill: parent
                                    radius: 10
                                    visible: false

                                }

                                OpacityMask {
                                    id: opacityMask2
                                    anchors.fill: mapImage2
                                    source: mapImage2
                                    maskSource: maskRectangle2
                                }
                            }

                            ColumnLayout
                            {
                                Label{
                                    text: "File name: " + model.fileName
                                }

                                Label{
                                    text: "Replay name: " + model.name
                                }

                                Label{
                                    text: "Mod: " + model.mod
                                }
                            }

                            Rectangle{
                                Layout.fillWidth: true
                            }
                        }

                        MouseArea{
                            id: delegateMouseArea
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true

                            onClicked: {
                                _uiBackend.replayManager.openPlayback(model.fileName);
                            }
                        }
                    }
                }
            }

            Rectangle{
                id: openPlaybackFolderButtonRectangle

                width: 300
                height: 35
                radius:5

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#337ab7"
                    }

                    GradientStop {
                        position: 1
                        color: "#245580"
                    }
                }

                RowLayout
                {
                    anchors.fill: parent
                    spacing: 15


                    Rectangle
                    {
                        width: 30
                        height: 30
                        color: "#00000000"

                       Layout.leftMargin: 15

                        Image{
                            id: iconImage
                            anchors.fill: parent
                            source: "qrc:/images/resources/images/folder.png"
                        }
                    }

                    Label{
                        id: openPlaybackFolderLabel
                        text: "Open playback folder"
                        Layout.rightMargin: 15

                        color: openPlaybackFolderMouseArea.containsMouse ? "#DCDCDC" : "#FFFFFF"
                        font.pixelSize: 16
                    }

                }

                MouseArea{
                    id: openPlaybackFolderMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true

                    onClicked: {
                        _uiBackend.replayManager.openPlaybackFolder();
                    }
                }
            }
        }
    }
}
