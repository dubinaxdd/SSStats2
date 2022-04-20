import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: infoRectangle
    opacity: 1
    color: "#ffffff"
    border.color: "#00000000"
    border.width: 0
    anchors.fill: parent
    Layout.margins: 60
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    property var model: _uiBackend.settingsPageModel

    ColumnLayout
    {
        anchors.margins: 15
        anchors.fill: parent

        GridLayout{

            columns: 4

            Label{
                text: "Russian fonts:"
            }

            Label{
                id: progressLabel0;
                text: model.russianFontsInstallProgress
            }

            Button{
                text: model.russianFontsInstalledStatus ? "Reinstall" : "Install"

                height: 20

                onClicked: {
                    model.installRussianFonts();
                }
            }

            Button{
                text: "Delete"
                height: 20
                enabled: model.russianFontsInstalledStatus

                onClicked: {
                    model.uninstallRussianFonts();
                }
            }



    /*
            Switch{
                text: "Install QWER hotkeys "
            }

            Switch{
                text: "Install advanced camera mod"
            }

            Switch{
                text: "Unlock races"
            }*/
        }

        Rectangle
        {
            Layout.fillHeight: true
        }

    }

    //Режим совместимости с win7
    //Установить русские шрифты
    //Установить QWER хоткеи
    //Установить камерамод
    //Разблокировать расы

    //Включить оверлей в игре
    //Маленькая игровая панель
    //Отключтить туман
    //Включить игровую панель
    //Масштаб оверлея


    radius: 10
}
