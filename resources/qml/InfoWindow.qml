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
                    text: "Welcome to DoW Stats Client"
                    font.pixelSize: 22 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: ""
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                }

                Label{
                    text: "Standard game settings for sending statistics:"
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
                            text: "   • Solo MMR game rules:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "1v1 game, Annihilate, Control Area, Take and Hold, Game Timer (optional)."
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Team MMR game rules:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Annihilate, Control Area (optional), Take and Hold (optional), Game Timer (optional)."
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Ai difficulty:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Optional, if there is AI in the game, statistics are not sent."
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Starting resources:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Standard"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }


                        Label{
                            text: "   • Lock teams:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Yes"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Enable cheats:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "No"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Starting location:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Optional"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Game speed:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Normal"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Resource sharing:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "No"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "   • Resource Rate:"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }

                        Label{
                            text: "Standard"
                            font.pixelSize: 14 * infoRectangle.sizeModifer
                            color: DowStatsStyle.textColor
                        }
                    }

                    Label{
                        text: "   • Game duration is more than 45 seconds"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }

                Label{
                    text: "Ranked mode:"
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Label{
                    text: "   A game is considered ranked when at least one of the players is playing in ranked mode."
                    font.pixelSize: 14 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }

                Label{
                    text: "Players are divided into the following ranks by Solo MMR"
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
                        text: "Division VII"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division VI"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division V"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division IV"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division III"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division II"
                        Layout.alignment: Qt.AlignHCenter
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "Division I"
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
                    text: "(these values ​​may vary depending on which ladder you are using)"
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 12 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }


                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }

                Label{
                    text: "Developers:"
                    font.pixelSize: 18 * infoRectangle.sizeModifer
                    color: DowStatsStyle.textColor
                }

                ColumnLayout
                {
                    spacing: 3  * infoRectangle.sizeModifer

                    Label{
                        text: "   Dubina_xdd - DoW Stats Client main developer"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Anibus - DoW Stats website main developer, DoW Stats Client developer"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Funeral - microservices developer"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   Jabka - DoW Stats website developer"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   New - first client (SSStats) main developer"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }

                    Label{
                        text: "   With participation: Elamaunt, TranspersonalPsy, Drobor, NCTribit, ZADGE, Nascent, YbuBaKa"
                        font.pixelSize: 14 * infoRectangle.sizeModifer
                        color: DowStatsStyle.textColor
                    }
                }

                Rectangle{
                    color:"#00000000"
                    Layout.preferredHeight: 30 * infoRectangle.sizeModifer
                }


                Label{
                    text: "DoW Stats Balance Mod Developers:"
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
