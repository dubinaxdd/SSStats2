import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: mainRectangle

    width: 280 * _uiBackend.sizeModifer

    Layout.minimumWidth: 280 * _uiBackend.sizeModifer
    Layout.maximumWidth: 280 * _uiBackend.sizeModifer

    color: "#00000000"
    border.color: "#00000000"

    property var model
    property real scrollViewPosition: 0
    property int playersCount: playersList.plyersCount
    property int relativeMouseX
    property int relativeMouseY

    function resetScrollView()
    {
        scrollView.setDefault();
    }

    Connections
    {
        target: _uiBackend


        function onSendExpand()
        {
            scrollViewPosition = 0.0;
        }
    }

    ScrollView {
        id: scrollView

        height: mainRectangle.height
        width: mainRectangle.width

        GlobalMouseArea
        {
            id: scrollViewMouseArea
            anchors.fill: parent

            onWheel: {
                if (delta > 0)
                    scrollView.scrollToTop();

                if (delta < 0)
                    scrollView.scrollToBottom()
            }
        }

        function scrollToBottom() {

            var cof = 0.04 * _uiBackend.sizeModifer


            if (ScrollBar.vertical.position + cof <= 0.9 && (!model.expandPatyStatistic || _uiBackend.expand) && (expandPatyStatisticButtonRectangle.y - (scrollView.height * (scrollViewPosition + cof))) >= 0.0 )
                ScrollBar.vertical.position += cof

            scrollViewPosition = ScrollBar.vertical.position
        }

        function scrollToTop() {

            var cof = 0.04 * _uiBackend.sizeModifer

            if ((!model.expandPatyStatistic || _uiBackend.expand) )
                ScrollBar.vertical.position -= cof

            if (ScrollBar.vertical.position < 0)
                ScrollBar.vertical.position = 0

            scrollViewPosition = ScrollBar.vertical.position
        }

        function setDefault()
        {
            ScrollBar.vertical.position = 0.0
            scrollViewPosition = 0.0
        }

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent

            spacing: 5 * _uiBackend.sizeModifer

            PlayersStatisticColumn
            {
                id: playersList
                hoverEnabled: false
                sizeModifer: _uiBackend.sizeModifer
                visible: !model.expandPatyStatistic || _uiBackend.expand
            }

            Rectangle {
                id: expandPatyStatisticButtonRectangle

                property bool howeredState: expandButton.hovered
                visible: _uiBackend.expandStatisticButtonVisible

                property Gradient grLight: Gradient {
                    GradientStop {
                        position: 0
                        color: "#428bca"
                    }

                    GradientStop {
                        position: 1
                        color: "#265a88"
                    }
                }

                property Gradient grDark: Gradient {
                    GradientStop {
                        position: 0
                        color: "#337ab7"
                    }

                    GradientStop {
                        position: 1
                        color: "#245580"

                    }
                }

                Layout.maximumWidth: 280 * _uiBackend.sizeModifer
                Layout.minimumWidth: 280 * _uiBackend.sizeModifer
                width: 280 * _uiBackend.sizeModifer

                Layout.maximumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                Layout.minimumHeight: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0
                height: !_uiBackend.expand ? 10 * _uiBackend.sizeModifer : 0

                radius: 5 * _uiBackend.sizeModifer

                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                //visible: !_uiBackend.expand
                gradient: howeredState ? grDark : grLight

                Image {
                    id: image

                    visible: !_uiBackend.expand
                    width: 95 * _uiBackend.sizeModifer
                    height: 10 * _uiBackend.sizeModifer
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/images/resources/images/expandDots.png"
                    anchors.horizontalCenter: parent.horizontalCenter
                    fillMode: Image.PreserveAspectFit
                }

                GlobalMouseArea{
                    id: expandButton
                    anchors.fill: parent

                    onClicked: {
                        _uiBackend.expandPatyStatisticButtonClick();
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color:"#00000000"
            }
        }
    }
}


