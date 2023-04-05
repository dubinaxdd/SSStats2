import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import Qt.labs.platform 1.1
import DowStatsStyle 1.0

Rectangle {
    id: infoRectangle
    opacity: 1
    color: DowStatsStyle.backgroundColor
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10

    onVisibleChanged:
    {
        findTextField.focus = false;
    }


    RowLayout
    {
        anchors.fill: parent

        ColumnLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 10

            spacing: 10

            RowLayout
            {
                TextField
                {
                    id: findTextField

                    Layout.fillWidth: true
                    selectByMouse: true
                    color: DowStatsStyle.textColor
                    font.pixelSize: 12

                    background: Rectangle {
                        radius: 10
                        implicitHeight: 40
                        border.color: "#00000000"
                        border.width: 0
                        color: DowStatsStyle.alternateBackgroundColor
                    }

                    onAccepted: {
                        findTextField.editingFinished();
                        _uiBackend.replayManager.findReplays(findTextField.text);
                    }

                    onEditingFinished:
                    {
                        focus = false;
                    }
                }

                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/find.svg"
                    toolTipText: "Find"

                    onClicked: {
                        findTextField.editingFinished();
                        _uiBackend.replayManager.findReplays(findTextField.text);
                    }
                }
            }

            RowLayout
            {
                spacing: 30
                Layout.preferredHeight: 140
                Layout.bottomMargin: 15

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
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                }

                ColumnLayout
                {
                    Layout.maximumWidth: 340
                    clip: true

                    Layout.alignment: Qt.AlignTop

                    spacing: 2

                    Label{
                        text: _uiBackend.replayManager.currentReplayName
                        font.pixelSize: 20
                        color: DowStatsStyle.textColor

                    }

                    Label{
                        id: fileNameLabel
                        text: "File name: " + _uiBackend.replayManager.currentFileName
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Mod: " + _uiBackend.replayManager.currentMod
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Creation time: " + _uiBackend.replayManager.currentReplayTime
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Duration: " + _uiBackend.replayManager.currentDuration
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Players count: " + _uiBackend.replayManager.currentPlayerCount
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Teams count: " + _uiBackend.replayManager.currentTeamsCount
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Map size: " + _uiBackend.replayManager.currentMapSize
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }
                }
            }


            RowLayout
            {
                Layout.alignment: Qt.AlignTop

                spacing: 30

                Label
                {
                    Layout.alignment: Qt.AlignTop
                    text: "AI difficulty: \nStarting resources: \nLock teams: \nEnable cheats: \nStarting location: \nGame speed: \nResource sharing: \nResource rate:"
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }

                Label
                {
                    Layout.alignment: Qt.AlignTop
                    text: _uiBackend.replayManager.gameSettings
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }

                Label
                {
                    Layout.alignment: Qt.AlignTop
                    text: _uiBackend.replayManager.winConditions
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }
            }

            ListView {
                id: playersListView

                Layout.topMargin: 20

                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollBar.vertical: ScrollBar {
                    id: scrollBar
                    visible: playersListView.contentItem.height > playersListView.height
                    policy: ScrollBar.AlwaysOn
                }

                clip: true

                model: _uiBackend.replayManager.playersListModel

                spacing: 5

                delegate:  Rectangle {

                    width: playersListView.width - (scrollBar.visible ? scrollBar.width + 5 : 0)
                    height: 50
                    radius: 10
                    color: model.type === "Observer" ? DowStatsStyle.highlightItemColor : model.color

                    GridLayout
                    {
                        anchors.fill: parent
                        anchors.margins: 10

                        columnSpacing: 10

                        columns: 6
                        rows: 1

                        Label {
                            text: model.type
                            Layout.alignment: Qt.AlignLeft
                            Layout.preferredWidth: 80
                            font.pixelSize: 11
                            color: DowStatsStyle.textColor
                        }

                        Label {
                            text: model.name
                            Layout.alignment: Qt.AlignLeft
                            //Layout.preferredWidth: 150
                            Layout.fillWidth: true
                            clip:true
                            font.pixelSize: 11
                            color: DowStatsStyle.textColor
                        }

                        Label {
                            text: model.type === "Observer" ? "" : "Race: " + model.race
                            Layout.alignment: Qt.AlignLeft
                            Layout.preferredWidth: 120
                            font.pixelSize: 11
                            color: DowStatsStyle.textColor
                        }

                        Label {
                            text: model.type === "Observer" ? "" : "Team: " + model.team
                            Layout.alignment: Qt.AlignLeft
                            Layout.preferredWidth: 60
                            font.pixelSize: 11
                            color: DowStatsStyle.textColor
                        }

                        Image
                        {
                            Layout.preferredWidth:30
                            Layout.preferredHeight: 30
                            cache: false
                            visible: model.type === "Observer" ? "" : "Team: " + model.team

                            source: "image://imageprovider/" + model.badgeUrl
                        }

                        Image
                        {
                            Layout.preferredWidth:20
                            Layout.preferredHeight: 30
                            cache: false
                            visible: model.type === "Observer" ? "" : "Team: " + model.team

                            source: "image://imageprovider/" + model.bannerUrl
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked:
                        {
                            if (model.type !== "Observer")
                            {
                                fullBadgesImagesRectangle.badgeUrl = ""
                                fullBadgesImagesRectangle.bannerUrl = ""
                                fullBadgesImagesRectangle.badgeUrl = model.badgeUrl
                                fullBadgesImagesRectangle.bannerUrl = model.bannerUrl
                                fullBadgesImagesRectangle.visible = true;
                                fullBadgesImagesRectangle.playerName = model.name
                            }
                        }
                    }
                }
            }

            RowLayout
            {
                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/save.svg"
                    toolTipText: "Save as..."

                    onClicked: {
                        saveFileDialog.currentFile =  "file:///" + _uiBackend.replayManager.currentFileName.replace('#', '');
                        saveFileDialog.visible = true;
                    }
                }

                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/delete.svg"
                    toolTipText: "Delete replay"

                    onClicked: {
                        _uiBackend.replayManager.removeReplay(_uiBackend.replayManager.currentFileName);
                    }
                }

                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/folder.svg"
                    toolTipText: "Choise other playback folder"

                    onClicked: {
                        choiseFolderDialog.visible = true
                    }
                }

                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/reset.svg"
                    toolTipText: "Choise default playback folder"

                    onClicked: {
                        _uiBackend.replayManager.choiseDefaultPlaybackFolder();
                    }
                }

                FileDialog {
                    id: saveFileDialog
                    fileMode: FileDialog.SaveFile
                    folder: _uiBackend.replayManager.currentFilePath
                    nameFilters: [ "Replay files (*.rec)" ]
                    visible: false
                    defaultSuffix: ""

                    onAccepted: {
                        _uiBackend.replayManager.saveReplay(currentFile);
                        visible = false;
                    }
                    onRejected: {
                        visible =  false;
                    }
                }

                FolderDialog
                {
                    id: choiseFolderDialog

                    onAccepted: {
                        _uiBackend.replayManager.choiseOtherPlaybackFolder(folder);
                        visible = false;
                    }

                    onRejected: {
                        visible =  false;
                    }
                }

                Rectangle
                {
                    Layout.fillWidth: true
                }
            }
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

            spacing: 10

            RowLayout
            {
                ComboBox{

                    id: sortComboBox

                    Layout.preferredWidth: 255
                    model: ["Sort by time", "Sort by file name", "Sort by name", "Sort by mod", "Sort by map" ]

                    onCurrentIndexChanged: _uiBackend.replayManager.replaysListModel.setSortType(currentIndex);

                    indicator: Rectangle{

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 10


                        width: 20
                        height: 20
                        color: "#00000000"

                        Image
                        {
                            id: image
                            anchors.fill: parent
                            anchors.margins: 3
                            source: "qrc:/images/resources/images/arrow_down.svg"
                            sourceSize.width: 14
                            sourceSize.height: 14
                        }

                        ColorOverlay{
                            anchors.fill: image
                            source:image
                            color: DowStatsStyle.textColor
                            //transform:rotation
                            antialiasing: true
                        }

                    }

                    contentItem: Text {
                        color: DowStatsStyle.textColor
                        text: "  " + sortComboBox.currentText
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: 12
                    }

                    background: Rectangle {
                        radius: 10
                        implicitHeight: 40
                        border.color: "#00000000"
                        border.width: 0
                        color: DowStatsStyle.alternateBackgroundColor
                    }
                }

                IconButton
                {
                    sourceUrl: "qrc:/images/resources/images/update.svg"
                    toolTipText: "Update"

                    onClicked: {
                        findTextField.text = "";
                        _uiBackend.replayManager.update();
                    }
                }
            }

            Rectangle
            {
                id: replaysListRectangle

                Layout.preferredWidth: 300
                Layout.fillHeight: true

                color: DowStatsStyle.alternateBackgroundColor
                radius: 10

                ListView{
                    id: replaysListView
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    spacing: 5

                    ScrollBar.vertical: ScrollBar {
                        id: replaysScrollBar
                        visible: replaysListView.contentItem.height > replaysListView.height
                        policy: ScrollBar.AlwaysOn
                    }

                    model: _uiBackend.replayManager.replaysListModel

                    delegate: Rectangle{

                        width: replaysListView.width - (replaysScrollBar.visible ? replaysScrollBar.width + 5 : 0)
                        height: 70
                        radius: 10

                        color: model.selected ? DowStatsStyle.selectionColor : delegateMouseArea.containsMouse ? DowStatsStyle.highlightItemColor : DowStatsStyle.backgroundColor

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
                                spacing: 2

                                Label{
                                    text: "File name: " + model.fileName
                                    font.pixelSize: 11
                                    color: DowStatsStyle.textColor
                                }

                                Label{
                                    text: "Replay name: " + model.name
                                    font.pixelSize: 11
                                    color: DowStatsStyle.textColor
                                }

                                Label{
                                    text: "Mod: " + model.mod
                                    font.pixelSize: 11
                                    color: DowStatsStyle.textColor
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
                                _uiBackend.replayManager.replaysListModel.setSelected(model.index);
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
                            source: "qrc:/images/resources/images/folder_white.svg"

                            sourceSize.width: 30
                            sourceSize.height: 30
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


    Rectangle{

        id: fullBadgesImagesRectangle

        anchors.fill: parent
        visible: false
        color: "#cf1a1919"
        radius: 10
        property string playerName: "UnknownPlayer";

        z: 10

        property string badgeUrl
        property string bannerUrl

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                fullBadgesImagesRectangle.visible = false;
            }
        }

        Rectangle
        {
            color: DowStatsStyle.backgroundColor
            radius: 10
            width:400
            height: 300
            anchors.horizontalCenter: fullBadgesImagesRectangle.horizontalCenter
            anchors.verticalCenter: fullBadgesImagesRectangle.verticalCenter

            ColumnLayout
            {
                anchors.fill: parent

                Rectangle
                {
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    Layout.topMargin: 15
                    color: DowStatsStyle.alternateBackgroundColor
                    radius: 10
                    width:300
                    height: 200

                    RowLayout {
                        id: attachmentRectangle

                        anchors.fill:parent
                        anchors.leftMargin: 15

                        spacing:15

                        Image {
                            id: badgeImage
                            cache: false
                            source: "image://imageprovider/" + fullBadgesImagesRectangle.badgeUrl

                            Layout.preferredWidth:120
                            Layout.preferredHeight: 120
                        }

                        Image {
                            id: bannerImage
                            cache: false
                            source: "image://imageprovider/" + fullBadgesImagesRectangle.bannerUrl

                            Layout.preferredWidth:120
                            Layout.preferredHeight: 180
                        }
                    }
                }

                RowLayout
                {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15

                    Button
                    {
                        text: "Save badge"

                        onClicked:
                        {
                            saveBadgesDialog.imageUrl = fullBadgesImagesRectangle.badgeUrl
                            saveBadgesDialog.imgWidth = 64
                            saveBadgesDialog.imgHeight = 64
                            saveBadgesDialog.folder = _uiBackend.replayManager.ssUrlPathPath + "/Badges"
                            saveBadgesDialog.currentFile = "file:///" + _uiBackend.replayManager.ssUrlPathPath + "/Banners/" + fullBadgesImagesRectangle.playerName.replace(" ", "_") +"_badge.tga"
                            saveBadgesDialog.visible = true;
                        }
                    }

                    Button
                    {
                        text: "Save banner"

                        onClicked:
                        {
                            saveBadgesDialog.imageUrl = fullBadgesImagesRectangle.bannerUrl
                            saveBadgesDialog.imgWidth = 64
                            saveBadgesDialog.imgHeight = 96
                            saveBadgesDialog.folder = _uiBackend.replayManager.ssUrlPathPath + "/Banners"
                            saveBadgesDialog.currentFile = "file:///" + _uiBackend.replayManager.ssUrlPathPath + "/Banners/" + fullBadgesImagesRectangle.playerName.replace(" ", "_") +"_banner.tga"
                            saveBadgesDialog.visible = true;
                        }
                    }
                }


                FileDialog
                {
                    id: saveBadgesDialog

                    property string imageUrl: " "
                    property int imgWidth: 0;
                    property int imgHeight: 0;

                    fileMode: FileDialog.SaveFile

                    nameFilters: [ "TGA (*.tga)" ]
                    defaultSuffix: ""

                    visible: false

                    onAccepted: {
                        _uiBackend.replayManager.saveBadges(saveBadgesDialog.currentFile, imageUrl, imgWidth, imgHeight)
                    }

                    onRejected: {
                        visible =  false;
                    }
                }
            }
        }
    }

}
