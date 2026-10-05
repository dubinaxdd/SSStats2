import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1
import DowStatsStyle 1.0

Item{

    Connections{
        target: _uiBackend


        function onRunLegacyOverlay()
        {
            console.log("ASDASDASDASDASD 111111")
            overlayWindow.show();
        }

        function onCloseLegacyOverlay()
        {
            console.log("ASDASDASDASDASD 222222")
            overlayWindow.close();
        }
    }



    Component.onCompleted: {
        mainWindow.show();
        overlayWindow.show();
    }

    MainWindow{
        id: mainWindow

        Component.onCompleted: {
            mainWindow.showNormal();
            mainWindow.raise();
            mainWindow.requestActivate();
        }
    }

    OverlayWindow
    {
        id: overlayWindow
    }

    SystemTrayIcon {
        id: trayIcon
        visible: true
        icon.source: "qrc:/icons/resources/icons/DowStatsClient.ico"

        onActivated:{
            mainWindow.showNormal();
            mainWindow.raise();
            mainWindow.requestActivate();
        }

        menu: Menu {
            MenuItem {
                text: qsTr("Exit")
                onTriggered: _uiBackend.onExit();
            }
        }
    }
}
