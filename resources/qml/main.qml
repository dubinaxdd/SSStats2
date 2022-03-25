import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import Qt.labs.platform 1.1

Item{
    Component.onCompleted: {
        mainWindow.show();
        overlayWindow.show();
    }

    MainWindow{
        id: mainWindow
    }

    OverlayWindow
    {
        id: overlayWindow
    }


    SystemTrayIcon {
        id: trayIcon
        visible: true
        icon.source: "qrc:/icons/resources/icons/ssstats.ico"

        onActivated:{
            mainWindow.show();
        }


        menu: Menu {
            MenuItem {
                text: qsTr("Launch Soulstorm with Windows7 support mode")

                onTriggered: _uiBackend.onLaunchSoulstormWithSupportMode();
            }

            MenuItem {
                text: qsTr("Exit")

                onTriggered: _uiBackend.onExit();
            }
        }
    }

}
