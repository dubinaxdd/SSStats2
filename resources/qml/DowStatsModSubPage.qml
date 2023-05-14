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

        ColumnLayout{
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label{
                text: model.currentModName
                color: DowStatsStyle.textColor
                font.pixelSize: 20
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

                TextArea{
                    id:changeLogTextArea

                    Layout.alignment: Qt.AlignTop
                    Layout.leftMargin: 15
                    Layout.rightMargin: 15
                    Layout.fillWidth: true

                    //text: mainRectangle.content
                    wrapMode: Text.Wrap
                    color: DowStatsStyle.textColor
                    font.pixelSize: 11
                    textFormat: Text.RichText

                    text: "Тут будет чейнджлог."
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
                            text: model.version
                            color: DowStatsStyle.textColor
                            font.pixelSize: 11
                        }

                        Label{
                            Layout.alignment: Qt.AlignVCenter
                            text: "Status: Not installed"
                            color: DowStatsStyle.textColor
                            font.pixelSize: 11
                        }
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
