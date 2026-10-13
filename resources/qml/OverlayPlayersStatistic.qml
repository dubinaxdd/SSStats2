
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
        scrollView.setDefault()
    }

    onPlayersCountChanged: resetScrollView()

    Connections {
        target: _uiBackend

        function onSendExpand()
        {
            scrollView.setDefault()
        }
    }

    ScrollView {
        id: scrollView

        anchors.fill: parent
        clip: true

        /*
         * ScrollView показывает содержимое Flickable.
         * Высота Flickable определяется только содержимым,
         * поэтому цикла привязки высоты не возникает.
         */
        contentWidth: flickable.width
        contentHeight: flickable.height

        Flickable {
            id: flickable

            width: scrollView.width
            height: scrollView.height

            contentWidth: width
            contentHeight: columnLayout.implicitHeight

            clip: true
            boundsBehavior: Flickable.StopAtBounds
            interactive: false

            ColumnLayout {
                id: columnLayout

                width: flickable.width
                spacing: 5 * _uiBackend.sizeModifer

                PlayersStatisticColumn {
                    id: playersList

                    hoverEnabled: false
                    sizeModifer: _uiBackend.sizeModifer

                    Layout.fillWidth: true

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

                    Layout.minimumWidth: 280 * _uiBackend.sizeModifer
                    Layout.maximumWidth: 280 * _uiBackend.sizeModifer
                    Layout.preferredWidth: 280 * _uiBackend.sizeModifer

                    Layout.minimumHeight:
                        !_uiBackend.expand
                        ? 10 * _uiBackend.sizeModifer : 0

                    Layout.maximumHeight:
                        !_uiBackend.expand
                        ? 10 * _uiBackend.sizeModifer : 0

                    Layout.preferredHeight:
                        !_uiBackend.expand
                        ? 10 * _uiBackend.sizeModifer : 0

                    radius: 5 * _uiBackend.sizeModifer

                    Layout.alignment: Qt.AlignRight | Qt.AlignTop

                    gradient: howeredState ? grDark : grLight

                    Image {
                        id: image

                        visible: !_uiBackend.expand

                        width: 95 * _uiBackend.sizeModifer
                        height: 10 * _uiBackend.sizeModifer

                        anchors.centerIn: parent

                        source: "qrc:/images/resources/images/expandDots.png"
                        fillMode: Image.PreserveAspectFit
                    }

                    GlobalMouseArea {
                        id: expandButton

                        anchors.fill: parent

                        onClicked: {
                            _uiBackend.expandPatyStatisticButtonClick()
                        }
                    }
                }
            }
        }

        /*
         * Обработка колеса мыши.
         */
        GlobalMouseArea {
            id: scrollViewMouseArea

            anchors.fill: parent

            onWheel: {
                if (delta > 0)
                    scrollView.scrollToTop()
                else if (delta < 0)
                    scrollView.scrollToBottom()
            }
        }

        /*
         * Максимальное смещение содержимого.
         */
        function maxScrollY()
        {
            return Math.max(
                0,
                flickable.contentHeight - height
            )
        }

        /*
         * Прокрутка вниз.
         */
        function scrollToBottom()
        {
            var maxY = maxScrollY()

            if (maxY <= 0)
                return

            var currentY = flickable.contentY
            var step = 30 * _uiBackend.sizeModifer

            /*
             * Координата верхней границы кнопки
             * в системе координат Flickable.
             */
            var buttonY =
                expandPatyStatisticButtonRectangle.y

            /*
             * Максимальное смещение, при котором
             * кнопка ещё не ушла выше верхней границы.
             */
            var maxButtonY = Math.max(0, buttonY)

            var targetY = Math.min(
                currentY + step,
                maxY,
                maxButtonY
            )

            flickable.contentY = Math.max(0, targetY)

            scrollViewPosition = maxY > 0
                ? flickable.contentY / maxY
                : 0
        }


        function scrollToTop()
        {
            var maxY = maxScrollY()

            if (maxY <= 0)
                return

            var step = 30 * _uiBackend.sizeModifer

            flickable.contentY = Math.max(
                flickable.contentY - step,
                0
            )

            scrollViewPosition = maxY > 0
                ? flickable.contentY / maxY
                : 0
        }

        function setDefault()
        {
            flickable.contentY = 0
            scrollViewPosition = 0
        }
    }
}
