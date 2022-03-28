import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

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

    property var model: _uiBackend.newsPage

    ColumnLayout
    {
        anchors.fill: parent

        ListView{
            id: messagesListView
            Layout.margins: 10
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 5

            model: infoRectangle.model

            delegate: DiscordMessage{
                width: messagesListView.width

                userName: model.userName
                content: model.content
                timesTamp: model.timesTamp
                avatarId: model.avatarId
            }
        }
    }
}
