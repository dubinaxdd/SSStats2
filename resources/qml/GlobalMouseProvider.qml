pragma Singleton
import QtQuick 2.0


QtObject {
    property var rootElement
    property real mouseX
    property real mouseY

    signal mouseClicked()

    function mouseClick(){
        mouseClicked();
    }
}
