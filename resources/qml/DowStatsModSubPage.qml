import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0

Rectangle {
    id: root
    color: "#00000000"
    property var model: _uiBackend.balanceModPage

    RowLayout
    {
        anchors.fill: parent
        spacing: 10

        ColumnLayout{
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label{
                text: model.selectedModName
                color: DowStatsStyle.textColor
                font.pixelSize: 20
            }

            Label{
                Layout.alignment: Qt.AlignVCenter
                text: "Version: " + model.selectedModVersion
                color: DowStatsStyle.textColor
                font.pixelSize: 11
            }


            RowLayout
            {
                Rectangle{
                    radius:5

                    Layout.preferredHeight: 10
                    Layout.preferredWidth: 10

                    color:  model.selectedModIsInstalled ? "#00ff99" : "#ffa9a9"
                }

                Label{
                    Layout.alignment: Qt.AlignVCenter
                    text: model.selectedModIsInstalled ? "Status: Installed" : "Status: Not installed"
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                }
            }

            RowLayout
            {
                Rectangle{
                    radius:5

                    Layout.preferredHeight: 10
                    Layout.preferredWidth: 10

                    color:  model.selectedModIsActual ? "#00ff99" : "#ffa9a9"
                }

                Label{
                    Layout.alignment: Qt.AlignVCenter
                    text: model.selectedModIsActual ? "This is latest version of this mod" : "This is an outdated version of this mod."
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                }
            }

            RowLayout
            {
                Rectangle{
                    radius:5

                    Layout.preferredHeight: 10
                    Layout.preferredWidth: 10

                    color:  model.selectedModIsCurrent ? "#00ff99" : "#ffa9a9"
                }

                Label{
                    Layout.alignment: Qt.AlignVCenter
                    text: model.selectedModIsCurrent ? "This mod is active in the game" : "This mod is not active in the game"
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                }
            }

            RowLayout{

                Layout.fillWidth: true;


                BlueButton {
                    text: "Install"
                }

                BlueButton {
                    text: "Uninstall"
                }

                BlueButton {
                    text: "Activate in game"
                    Layout.preferredWidth: 100

                }

                Item {
                    Layout.fillWidth: true
                }
            }

            Item{
                Layout.preferredHeight: 60
            }

            Label{
                text: "Changelog"
                color: DowStatsStyle.textColor
                font.pixelSize: 14
            }

            Rectangle{
                radius: 10
                color: DowStatsStyle.alternateBackgroundColor
                Layout.fillWidth: true
                Layout.fillHeight: true


                Flickable
                {
                    anchors.fill: parent
                    anchors.margins: 15

                    contentHeight: changeLogTextArea.height
                    clip: true

                    TextArea{
                        id:changeLogTextArea
                        width: parent.width - (scrollBar.visible ? scrollBar.width : 0)

                        wrapMode: Text.Wrap
                        color: DowStatsStyle.textColor
                        font.pixelSize: 11
                        //textFormat: Text.RichText
                        selectByMouse: true
                        readOnly: true

                        text: model.selectedChangeLog
                    }

                    ScrollBar.vertical: ScrollBar {
                        id: scrollBar
                        visible: changeLogTextArea.height > parent.height
                        policy: ScrollBar.AlwaysOn
                    }
                }
            }
        }

        Rectangle
        {
            Layout.fillHeight: true
            Layout.preferredWidth: 200
            radius: 10
            color: DowStatsStyle.alternateBackgroundColor

            ListView
            {
                id: listView
                anchors.fill: parent
                anchors.margins: 10
                spacing: 5

                clip: true

                model: root.model

                delegate: Rectangle{
                    width: listView.width
                    height: 70
                    radius: 10

                    color: model.selected ? DowStatsStyle.selectionColor : delegateMouseArea.containsMouse ? DowStatsStyle.highlightItemColor : DowStatsStyle.itemColor

                    ColumnLayout
                    {
                        anchors.fill: parent
                        anchors.margins: 10

                        spacing: 3

                        Label{
                            Layout.alignment: Qt.AlignVCenter
                            text: model.uiName
                            color: DowStatsStyle.textColor
                            font.pixelSize: 11
                        }

                        Label{
                            Layout.alignment: Qt.AlignVCenter
                            text: "Version: " + model.version
                            color: DowStatsStyle.textColor
                            font.pixelSize: 11
                        }


                        RowLayout
                        {
                            Rectangle{
                                radius:5

                                Layout.preferredHeight: 10
                                Layout.preferredWidth: 10

                                color:  model.isInstalled ? "#00ff99" : "#ffa9a9"
                            }

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: model.isInstalled ? "Status: Installed" : "Status: Not installed"
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11
                            }
                        }

/*
                        RowLayout
                        {
                            Rectangle{
                                radius:5

                                Layout.preferredHeight: 10
                                Layout.preferredWidth: 10

                                color:  model.isCurrentMod ? "#00ff99" : "#ffa9a9"
                            }

                            Label{
                                Layout.alignment: Qt.AlignVCenter
                                text: model.isCurrentMod ? "Is current mod in game" : "Is not current mod in game"
                                color: DowStatsStyle.textColor
                                font.pixelSize: 11
                            }
                        }*/
                    }

                    MouseArea{
                        id:delegateMouseArea
                        anchors.fill: parent
                        hoverEnabled: true;

                        onClicked: root.model.slectItem(model.index);
                    }
                }
            }
        }
    }
}
