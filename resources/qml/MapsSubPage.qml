import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1
import DowStatsStyle 1.0

Rectangle {

    color: "#00000000"

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 20

        RowLayout
        {
            Layout.preferredHeight: miniMapRectangle.height
            Layout.fillWidth: true

            spacing: 20

            ColumnLayout
            {
                Layout.alignment: Qt.AlignTop

                Rectangle
                {
                    id: miniMapRectangle

                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 140

                    Layout.alignment: Qt.AlignTop

                    color: "#112332"
                    radius: 10
                    clip: true

                    //Костыль для перезагрузки картинки, рил так на формух делают
                    Connections {
                        target: _uiBackend.mapManagerPage

                        function onCurrentMapNameChanged()
                        {
                            if (_uiBackend.mapManagerPage.currentMapNeedInstall)
                            {
                                mapImage.source = "";
                                mapImage.source = "image://imageprovider/" + _uiBackend.mapManagerPage.currentMiniMapId;
                            }
                            else
                            {
                                mapImage.source = "";
                                mapImage.source = "image://imageprovider/currentMiniMap";
                            }
                        }
                    }

                    Image{
                        id: mapImage
                        anchors.fill: parent
                        source: "image://imageprovider/currentMiniMap"
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
                        text: qsTr("Checking updates...")
                        visible: _uiBackend.mapManagerPage.loadMapInfoProcessed

                        font.pixelSize: 11
                    }

                    OpacityMask {
                        id: opacityMask
                        anchors.fill: mapImage
                        source: mapImage
                        maskSource: maskRectangle
                        visible: !_uiBackend.mapManagerPage.loadMapInfoProcessed
                    }

                }

                Label
                {
                    id: downloadingProgress
                    visible: _uiBackend.mapManagerPage.downloadedProcessed
                    text: qsTr("Downloading progress: ") + _uiBackend.mapManagerPage.downloadedCount + "/" + _uiBackend.mapManagerPage.fullCount
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }
            }

            ColumnLayout
            {
                Layout.alignment: Qt.AlignTop

                Label{
                    text: _uiBackend.mapManagerPage.currentMapName
                    font.pixelSize: 20
                    Layout.preferredWidth: 190
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: qsTr("Authors: ") + _uiBackend.mapManagerPage.currentMapAuthors
                    Layout.preferredWidth: 190
                    visible: _uiBackend.mapManagerPage.currentMapAuthors !== ""
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: _uiBackend.mapManagerPage.currentMapDescription
                    Layout.preferredWidth: 190
                    visible: _uiBackend.mapManagerPage.currentMapDescription !== ""
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: qsTr("Tags: ") + _uiBackend.mapManagerPage.currentMapTags
                    Layout.preferredWidth: 190
                    visible: _uiBackend.mapManagerPage.currentMapTags !== ""
                    font.pixelSize: 11
                    color: DowStatsStyle.textColor
                }
            }

            Rectangle
            {
                Layout.fillWidth: true
            }

            ColumnLayout
            {
                Layout.alignment: Qt.AlignTop

                BlueButton
                {
                    Layout.preferredWidth: 220
                    text: qsTr("Check updates")
                    enabled: !(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        _uiBackend.mapManagerPage.loadMapsInfo();
                    }
                }


                BlueButton
                {
                    Layout.preferredWidth: 220
                    text: qsTr("Install default maps")
                    enabled: !(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        if (_uiBackend.ssLaunchState)
                            _uiBackend.soulstormLaunchedDialogVisible = true
                        else
                            _uiBackend.mapManagerPage.installDefaultMaps();
                    }
                }

                BlueButton
                {
                    Layout.preferredWidth: 220
                    text: qsTr("Install all maps")
                    enabled: !(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        if (_uiBackend.ssLaunchState)
                            _uiBackend.soulstormLaunchedDialogVisible = true
                        else
                            _uiBackend.mapManagerPage.installAllMaps();
                    }
                }

                StyledSwitch{
                    text: qsTr("Auto-install default maps")
                    checked: _uiBackend.mapManagerPage.autoinstallDefaultMaps
                    onCheckedChanged: _uiBackend.mapManagerPage.autoinstallDefaultMaps = checked
                }

                StyledSwitch{
                    text: qsTr("Auto-install all maps")
                    checked: _uiBackend.mapManagerPage.autoinstallAllMaps
                    onCheckedChanged: _uiBackend.mapManagerPage.autoinstallAllMaps = checked
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
                visible: mapsListView.contentItem.height > mapsListView.height
                policy: ScrollBar.AlwaysOn
            }

            model: _uiBackend.mapManagerPage

            spacing: 5

            delegate:  Rectangle {

                width: mapsListView.width - scrollBar.width
                height: 50
                radius: 10
                //color: model.selected ? "#A9A9A9" : delegateMouseArea.containsMouse ? "#c8c8c8" : "#DCDCDC"
                color: model.selected ? DowStatsStyle.selectionColor : delegateMouseArea.containsMouse ? DowStatsStyle.highlightItemColor : DowStatsStyle.itemColor

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
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Label
                    {
                        Layout.preferredWidth: 60
                        text: model.description
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                        //visible: model.description !== ""
                    }

                    Label
                    {
                        Layout.preferredWidth: 100
                        text: qsTr("Tags: ") + model.tags
                        visible: model.tags !== ""
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    Rectangle
                    {
                        Layout.fillWidth: true
                    }

                    Label
                    {
                        text: qsTr("Downloading processed...")
                        visible: model.downloadingProcessed
                        font.pixelSize: 11
                        color: DowStatsStyle.textColor
                    }

                    BlueButton{
                        text: qsTr("Install")
                        z:2
                        visible: model.needInstall && !model.downloadingProcessed
                        onClicked: _uiBackend.mapManagerPage.installMap(model.index)
                    }

                    BlueButton{
                        text: qsTr("Update")
                        z:2
                        visible: !model.needInstall && model.needUpdate && !model.downloadingProcessed
                        onClicked:{
                            if (_uiBackend.ssLaunchState)
                                _uiBackend.soulstormLaunchedDialogVisible = true
                            else
                                _uiBackend.mapManagerPage.installMap(model.index)
                        }
                    }

                    BlueButton{
                        text: qsTr("Delete")
                        z:2
                        visible: !model.needInstall && !model.needUpdate && !model.downloadingProcessed
                        onClicked: {
                            if (_uiBackend.ssLaunchState)
                                _uiBackend.soulstormLaunchedDialogVisible = true
                            else
                                _uiBackend.mapManagerPage.removeMap(model.index);
                        }
                    }
                }
            }
        }
    }
}
