import QtQuick 2.0
import GlobalMouseProvider 1.0

Item {
    id: root

    property real globalX: 0
    property real globalY: 0

    property bool hovered: false

    property Rectangle rootElement: GlobalMouseProvider.rootElement
    property real mouseX: GlobalMouseProvider.mouseX
    property real mouseY: GlobalMouseProvider.mouseY

    signal clicked()
    signal wheel(int delta)

    onMouseXChanged: mouseCoordinatesChanged();
    onMouseYChanged: mouseCoordinatesChanged();
    Component.onCompleted: mouseCoordinatesChanged();

    Connections{
        target: rootElement

        function onWidthChanged(){
            mouseCoordinatesChanged();
        }

        function onHeightChanged(){
            mouseCoordinatesChanged();
        }
    }

    Connections{
        target: GlobalMouseProvider

        function onMouseClicked(){
            if (root.height > 0 && root.width > 0 && root.visible)
                mouseClicked();
        }

        function onMouseWheel(delta){
            if (root.height > 0 && root.width > 0 && root.visible)
                mouseWheel(delta);
        }
    }

    function updateCoordinates(){
        var globalCoordinares = root.mapToItem(rootElement, 0, 0);
        globalX = globalCoordinares.x;
        globalY = globalCoordinares.y;
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

    function mouseWheel(delta)
    {
        if (mouseX >= globalX &&
                mouseX <= globalX + width &&
                mouseY >= globalY &&
                mouseY <= globalY + height)
        {
            wheel(delta)
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
            if(!hovered)
                hovered = true;
        }
        else
        {
            if(hovered)
                hovered = false;
        }
    }
}
