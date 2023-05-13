import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
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

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Rectangle
        {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            //Layout.topMargin: 15
            color: DowStatsStyle.alternateBackgroundColor
            radius: 10
            Layout.preferredWidth: 220
            Layout.fillHeight: true

            ColumnLayout
            {
                anchors.fill: parent
                anchors.margins: 10

                SubPageMenuButton
                {
                    id: balanceModButton

                    text: "DowStats Balance Mod"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    pressedState: true

                    onClicked: {
                        mapsButton.pressedState = false;
                        otherModsButton.pressedState = false;

                        dowStatsModSubPage.visible = true;
                        mapsSubPage.visible = false;
                        otherModsSubPage.visible = false;
                    }
                }

                SubPageMenuButton
                {
                    id: mapsButton
                    text: "Maps"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                    newsAvailable: _uiBackend.mapManagerPage.updatesAvailable

                    onClicked: {
                        balanceModButton.pressedState = false;
                        otherModsButton.pressedState = false;

                        dowStatsModSubPage.visible = false;
                        mapsSubPage.visible = true;
                        otherModsSubPage.visible = false;
                    }
                }

                SubPageMenuButton
                {
                    id: otherModsButton
                    text: "Other mods"
                    Layout.preferredWidth: 200
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    onClicked: {
                        balanceModButton.pressedState = false;
                        mapsButton.pressedState = false;

                        dowStatsModSubPage.visible = false;
                        mapsSubPage.visible = false;
                        otherModsSubPage.visible = true;
                    }
                }

                Rectangle{
                    Layout.fillHeight: true
                }
            }
        }

        ColumnLayout
        {
            DowStatsModSubPage
            {
                id: dowStatsModSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: true
            }

            MapsSubPage
            {
                id: mapsSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: false
            }

            OtherModsSubPage
            {
                id: otherModsSubPage
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: false
            }
        }
    }
}
