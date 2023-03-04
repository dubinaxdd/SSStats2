import QtQuick 2.0

Item {

    property real globalX: 0
    property real globalY: 0

    property bool howered: false

    property var rootElement
    property real mouseX: 0
    property real mouseY: 0

    signal clicked()

    onMouseXChanged: mouseCoordinatesChanged();
    onMouseYChanged: mouseCoordinatesChanged();
    Component.onCompleted: mouseCoordinatesChanged();

    function updateCoordinates(){
        var globalCoordinares = expandPatyStatisticButtonRectangle.mapToItem(windowRectangle, 0, 0);
        globalX = globalCoordinares.x;
        globalY = globalCoordinares.y;

        console.log("ASDASDASDADASDASDASD", globalX, globalY, x, y)
    }

    function mouseClicked()
    {


        if (mouseX >= globalX &&
                mouseX <= globalX + width &&
                mouseY >= globalY &&
                mouseY <= globalY + height)
        {
            clicked();
        }
    }

    function mouseCoordinatesChanged()
    {
        updateCoordinates();

        if (mouseX >= globalX &&
                mouseX <= globalX + width &&
                mouseY >= globalY &&
                mouseY <= globalY + height)
        {
            if(!howered)
                howered = true;
        }
        else
        {
            if(howered)
                howered = false;
        }
    }
}
