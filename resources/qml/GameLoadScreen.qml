import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: root
    color: "#00000000"
    border.color: "#00000000"

    PlayersStatisticColumn
    {
        id: playersListRight

        anchors.top: parent.top
        //anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 280 * _uiBackend.sizeModifer

        anchors.topMargin: 15 * _uiBackend.sizeModifer

        hoverEnabled: false
        sizeModifer: _uiBackend.sizeModifer
        columnPosition: "right"
    }

    PlayersStatisticColumn
    {
        id: playersListLeft

        anchors.top: parent.top
        //anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: 280 * _uiBackend.sizeModifer

        hoverEnabled: false
        sizeModifer: _uiBackend.sizeModifer
        columnPosition: "left"
    }
}

