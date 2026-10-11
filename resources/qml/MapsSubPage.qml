import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.15
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1
import DowStatsStyle 1.0

Rectangle {

    color: "#00000000"

    ColumnLayout {
        anchors.fill: parent
        spacing: 20

        // =================================================
        // Кнопки управления
        // =================================================

        RowLayout {
            id: controlPanel
            Layout.fillWidth: true
            spacing: 20
            Layout.alignment: Qt.AlignTop

            ColumnLayout
            {
                BlueButton {
                    Layout.preferredWidth: 220
                    text: qsTr("Check updates")
                    enabled:!(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        _uiBackend.mapManagerPage.loadMapsInfo();
                    }
                }

                BlueButton {
                    Layout.preferredWidth: 220
                    text: qsTr("Install default maps")
                    enabled: !(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        if (_uiBackend.gameLaunchState)
                            _uiBackend.soulstormLaunchedDialogVisible = true
                        else
                            _uiBackend.mapManagerPage.installDefaultMaps();
                    }
                }

                BlueButton {
                    Layout.preferredWidth: 220
                    text: qsTr("Install all maps")
                    enabled: !(_uiBackend.mapManagerPage.downloadedProcessed || _uiBackend.mapManagerPage.loadMapInfoProcessed)

                    onClicked: {
                        if (_uiBackend.gameLaunchState)
                            _uiBackend.soulstormLaunchedDialogVisible = true
                        else
                            _uiBackend.mapManagerPage.installAllMaps();
                    }
                }
            }

            ColumnLayout
            {
                //Layout.preferredHeight: parent.height
                Layout.alignment: Qt.AlignTop

                StyledSwitch {
                    text: qsTr("Auto-install default maps")
                    checked: _uiBackend.mapManagerPage.autoinstallDefaultMaps

                    onCheckedChanged: {
                        _uiBackend.mapManagerPage.autoinstallDefaultMaps = checked;
                    }
                }

                StyledSwitch {
                    text: qsTr("Auto-install all maps")
                    checked: _uiBackend.mapManagerPage.autoinstallAllMaps

                    onCheckedChanged: {
                        _uiBackend.mapManagerPage.autoinstallAllMaps = checked;
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
        }


        // =====================================================
        // Сетка карт
        // =====================================================

        GridView {
            id: mapsGridView
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            model: _uiBackend.mapManagerPage

            // Размер одной ячейки
            cellWidth: 210
            cellHeight: 310

            ScrollBar.vertical: ScrollBar {
                id: scrollBar
                policy: ScrollBar.AsNeeded
            }

            delegate: Rectangle {

                width: 200
                height: 300

                radius: 10

                color:
                    model.selected
                    ? DowStatsStyle.selectionColor
                    : delegateMouseArea.containsMouse
                      ? DowStatsStyle.highlightItemColor
                      : DowStatsStyle.itemColor

                border.width:
                    model.needInstall || model.needUpdate
                    ? 1
                    : 0

                border.color: "#D33"

                // =================================================
                // Выбор карты
                // =================================================

                MouseArea {
                    id: delegateMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        _uiBackend.mapManagerPage.selectMap(model.index);
                    }
                }


                // =================================================
                // Содержимое карточки
                // =================================================

                ColumnLayout {

                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 7

                    // =============================================
                    // Превью карты
                    // =============================================

                    Rectangle {
                        id: previewRectangle
                        Layout.fillWidth: true
                        Layout.preferredHeight: width
                        radius: 7
                        color: "#112332"
                        clip: true

                        Image {
                            id: previewImage
                            anchors.fill: parent

                            source:
                                model.miniMapId !== undefined
                                && model.miniMapId !== ""
                                ? "image://imageprovider/"
                                  + model.miniMapId
                                : ""

                            cache: false
                            asynchronous: true
                            fillMode: Image.PreserveAspectCrop
                            visible: status === Image.Ready
                        }


                        // =========================================
                        // Если картинка ещё не загрузилась
                        // =========================================

                        Label {
                            anchors.centerIn: parent
                            text: qsTr("No preview")
                            color: "#FFFFFF"
                            font.pixelSize: 11
                            visible: previewImage.status !== Image.Ready
                        }

                        // =========================================
                        // Индикатор необходимости установки
                        // =========================================

                        Rectangle {
                            width: 13
                            height: 13

                            radius: 7

                            anchors {
                                top: parent.top
                                right: parent.right

                                margins: 7
                            }

                            color: "#E53935"

                            visible: model.needInstall || model.needUpdate
                        }


                        // =========================================
                        // Затемнение при наведении
                        // =========================================

                        Rectangle {
                            anchors.fill: parent
                            radius: 7
                            color: "#000000"

                            opacity:
                                delegateMouseArea.containsMouse
                                ? 0.15
                                : 0

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 100
                                }
                            }
                        }
                    }


                    // =================================================
                    // Название карты
                    // =================================================

                    Label {
                        Layout.fillWidth: true

                        text: model.mapName
                        font.pixelSize: 14
                        font.bold: true
                        color: DowStatsStyle.textColor
                        elide: Text.ElideRight
                        maximumLineCount: 1
                    }


                    // =================================================
                    // Описание
                    // =================================================

                    Label {
                        Layout.fillWidth: true
                        text: model.description
                        font.pixelSize: 10
                        color: DowStatsStyle.textColor
                        maximumLineCount: 2
                        wrapMode: Text.WordWrap
                        elide: Text.ElideRight
                        visible: model.description !== ""
                    }

                    // =================================================
                    // Теги
                    // =================================================

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Tags: ") + model.tags
                        font.pixelSize: 10
                        color: DowStatsStyle.textColor
                        elide: Text.ElideRight
                        visible: model.tags !== ""
                    }


                    // Заполнитель
                    Item {
                        Layout.fillHeight: true
                    }


                    // =================================================
                    // Статус загрузки
                    // =================================================

                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Downloading processed...")
                        font.pixelSize: 10
                        color: DowStatsStyle.textColor
                        horizontalAlignment: Text.AlignHCenter
                        visible: model.downloadingProcessed
                    }


                    // =================================================
                    // Кнопки
                    // =================================================

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5

                        // =============================================
                        // Install
                        // =============================================

                        BlueButton {
                            Layout.fillWidth: true
                            text: qsTr("Install")

                            visible: model.needInstall && !model.downloadingProcessed

                            onClicked: {
                                _uiBackend.mapManagerPage.installMap(model.index);
                            }
                        }


                        // =============================================
                        // Update
                        // =============================================

                        BlueButton {
                            Layout.fillWidth: true

                            text: qsTr("Update")

                            visible:
                                !model.needInstall
                                && model.needUpdate
                                && !model.downloadingProcessed

                            onClicked: {
                                if (_uiBackend.gameLaunchState) {
                                    _uiBackend.soulstormLaunchedDialogVisible =
                                        true;
                                }
                                else {
                                    _uiBackend.mapManagerPage.installMap(
                                        model.index
                                    );
                                }
                            }
                        }


                        // =============================================
                        // Delete
                        // =============================================

                        BlueButton {
                            Layout.fillWidth: true
                            text: qsTr("Delete")

                            visible:
                                !model.needInstall
                                && !model.needUpdate
                                && !model.downloadingProcessed

                            onClicked: {
                                if (_uiBackend.gameLaunchState) {
                                    _uiBackend.soulstormLaunchedDialogVisible =
                                        true;
                                }
                                else {
                                    _uiBackend.mapManagerPage.removeMap(
                                        model.index
                                    );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
