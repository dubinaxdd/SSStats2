import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import DowStatsStyle 1.0


Rectangle {
    id: infoRectangle

    property real sizeModifer: 1
    property bool isOverlayPage: false
    property var model: _uiBackend.informationPage

    opacity: 1
    color: DowStatsStyle.backgroundColor
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60 * infoRectangle.sizeModifer
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    radius: 10 * infoRectangle.sizeModifer

    Flickable
    {
        id: scrollView

        anchors.fill: parent
        anchors.margins: 10 * infoRectangle.sizeModifer
        clip: true

        contentHeight: content.height

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            visible: content.height > scrollView.height
            policy: ScrollBar.AlwaysOn
        }

        GlobalMouseArea
        {
            id: scrollViewMouseArea
            anchors.fill: parent
            enabled: infoRectangle.isOverlayPage

            onWheel: {
                if (!infoRectangle.isOverlayPage)
                    return;

                if (delta > 0)
                    scrollView.scrollToTop();

                if (delta < 0)
                    scrollView.scrollToBottom()
            }
        }

        function scrollToBottom() {

            var cof = 0.04 * _uiBackend.sizeModifer

            if (ScrollBar.vertical.position + cof <= 0.9 && scrollView.height + scrollView.y < scrollView.contentItem.y + scrollView.contentItem.height)
                ScrollBar.vertical.position += cof
        }

        function scrollToTop() {

            var cof = 0.04 * _uiBackend.sizeModifer

            if (ScrollBar.vertical.position - cof > 0  )
                ScrollBar.vertical.position -= cof
            else
                ScrollBar.vertical.position = 0
        }

        ColumnLayout
        {
            id: content
            width: scrollView.width
            ColumnLayout
            {
                id: columnLayout
                Layout.alignment: Qt.AlignHCenter

                Label{
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("Welcome to DoW Stats Client")
                    font.pixelSize: 22 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: qsTr("The client is designed to collect statistics and replays for Dawn of War - Soulstorm and Dawn of War - Definitive Edition.")
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: qsTr("Leaderboards and replays can be found on dowstats.ru or dowstats.com.")
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: ""
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                }

                Label{
                    text: qsTr("Players are divided into the following ranks by Solo MMR")
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    Layout.alignment: Qt.AlignHCenter
                    color: DowStatsStyle.textColor
                }

                GridLayout
                {
                    Layout.alignment: Qt.AlignHCenter
                    columns: 7
                    columnSpacing: 20  * infoRectangle.sizeModifer

                    Label{
                        text: qsTr("Division VII")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division VI")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division V")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division IV")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division III")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division II")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("Division I")
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "-" + model.rank7Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank6Min + " - " + model.rank6Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank5Min + " - " + model.rank5Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank4Min + " - " + model.rank4Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank3Min + " - " + model.rank3Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank2Min + " - " + model.rank2Max
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: model.rank1Min + "+ "
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank1.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank3.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank4.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank5.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank6.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank7.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }

                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: 60 * infoRectangle.sizeModifer
                        Layout.preferredHeight: 60 * infoRectangle.sizeModifer

                        source: "qrc:/images/resources/images/Rank8.svg"
                        sourceSize.width: 60 * infoRectangle.sizeModifer
                        sourceSize.height: 60 * infoRectangle.sizeModifer
                    }
                }

                Label{
                    text: qsTr("(these values ​​may vary depending on which ladder you are using)")
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 12 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: qsTr("   A game is considered ranked when at least one of the players is playing in ranked mode.")
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                    Layout.alignment: Qt.AlignHCenter
                    color: DowStatsStyle.textColor
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }


                Label{
                    text: qsTr("Standard game settings for sending statistics:")
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                ColumnLayout
                {
                    spacing: 3  * infoRectangle.sizeModifer

                    GridLayout
                    {
                        columns: 2
                        rowSpacing: 3  * infoRectangle.sizeModifer


                        Label{
                            text: qsTr("Definitive Edition:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Solo MMR game rules:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("1v1 game, Automatch, Annihilate, Control Area, Take and Hold, Game Timer (optional).")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Team MMR game rules:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Automatch, Annihilate, Control Area, Take and Hold, Game Timer (optional).")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }


                        Label{
                            text: qsTr("   • Custom MMR game rules:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Annihilate, Control Area (optional), Take and Hold (optional), Game Timer (optional).")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }


                        Label{
                            text: qsTr("Soulstorm and supported mods:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Solo MMR game rules:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("1v1 game, Annihilate, Control Area, Take and Hold, Game Timer (optional).")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Team MMR game rules:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Annihilate, Control Area (optional), Take and Hold (optional), Game Timer (optional).")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("General:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Ai difficulty:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Optional, if there is AI in the game, statistics are not sent.")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Starting resources:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Standard")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }


                        Label{
                            text: qsTr("   • Lock teams:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Yes")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Enable cheats:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("No")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Starting location:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Optional")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Game speed:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Normal")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Resource sharing:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("No")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Resource Rate:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("Standard")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("   • Disable Flyers:")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: qsTr("No")
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }
                    }

                    Label{
                        text: qsTr("   • Game duration is more than 45 seconds")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }

                Label{
                    text: qsTr("Developers:")
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                ColumnLayout
                {
                    spacing: 3  * infoRectangle.sizeModifer

                    Label{
                        text: qsTr("   Dubina_xdd - DoW Stats Client main developer")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("   Anibus - DoW Stats website main developer, DoW Stats Client developer")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("   Funeral - microservices developer")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("   Jabka - DoW Stats website developer")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("   New - first client (SSStats) main developer")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: qsTr("   With participation: " + "Elamaunt, TranspersonalPsy, Drobor, NCTribit, ZADGE, Nascent, YbuBaKa")
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }


                Label{
                    text: qsTr("DoW Stats Balance Mod Developers:")
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                ColumnLayout
                {
                    spacing: 3  * infoRectangle.sizeModifer

                    Label{
                        text: "   Master Yoba"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Nascent"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Tornado"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Tr1v"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Yang Wen-li"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   ZADGE"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }
            }
        }
    }
}
