import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: infoRectangle
    width: 600
    height: 600
    opacity: 1
    color: "#00000000"
    border.color: "#00000000"
    transformOrigin: Item.BottomRight
    Layout.fillWidth: true
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom
    Layout.fillHeight: true

    Rectangle {
        id: rectangle
        opacity: 1
        color: "#f9f9f9"
        border.color: "#000000"
        border.width: 3
        anchors.fill: parent
        Layout.margins: 60
        Layout.fillWidth: true
        Layout.fillHeight: true

        Text {
            id: infoText
            text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<h2 align=\"center\" style=\" margin-top:20px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:110%; background-color:#ffffff;\"><span style=\" font-size:x-large; font-weight:600; background-color:#ffffff;\">ИНФОРМАЦИЯ</span></h2>\n<h2 style=\" margin-top:20px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:110%; background-color:#ffffff;\"><span style=\" font-family:'Helvetica Neue','Helvetica','Arial','sans-serif'; font-size:x-large; font-weight:496; color:#333333; background-color:#ffffff;\">Проблемы и решения</span></h2>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" font-family:'Helvetica Neue','Helvetica','Arial','sans-serif'; font-size:14px; color:#333333;\" style=\" margin-top:0px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a name=\"info\"></a><span style=\" font-size:14px;\">П</span><span style=\" font-size:14px;\">осле переустановки игры или проверки кэша необходимо переустановить сборщик статистики, потому что файл, ответственный за его запуск, заменен!</span></li>\n<li style=\" font-family:'Helvetica Neue','Helvetica','Arial','sans-serif'; font-size:14px; color:#333333;\" style=\" margin-top:0px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a name=\"info\"></a><span style=\" font-size:14px;\">В</span><span style=\" font-size:14px;\">ы также должны убедиться, что статистика включена. Для этого откройте файл stats.ini, проверьте параметр «enableStats», он должен быть равен 1</span></li>\n<li style=\" font-family:'Helvetica Neue','Helvetica','Arial','sans-serif'; font-size:14px; color:#333333;\" style=\" margin-top:0px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a name=\"info\"></a><span style=\" font-size:14px;\">Е</span><span style=\" font-size:14px;\">сли ваша статистика по-прежнему не работает, переустановите ее с помощью </span><a href=\"http://dowstats.ru/ssstats/SSStats.zip\"><span style=\" font-size:14px; text-decoration: underline; color:#428bca; background-color:transparent;\">последней версии сборщика статистики!</span></a></li>\n<li style=\" font-family:'Helvetica Neue','Helvetica','Arial','sans-serif'; font-size:14px; color:#333333;\" style=\" margin-top:0px; margin-bottom:10px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14px;\">Если после установки сборщика статистики игра не запускается, возможно проблема в том что версия 1.0.8 устанавливает режим совместимости с Windows XP SP3 для файла Soulstorm.exe, это было сделано для корректной работы DirectX хука, который используется для отображения статистики игроков на экране. Для того чтобы убрать режим совместимости нажмите правой кнопкой мыши на файл Soulstorm.exe, выберите пункт &quot;Свойства&quot;, затем перейдите на вкладку &quot;Совместимость&quot; и снимите галочку напротив пункта &quot;Запускать программу в режиме совместимости с&quot;.</span></li></ul></body></html>"
            anchors.fill: parent
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            anchors.rightMargin: 10
            anchors.leftMargin: 10
            anchors.bottomMargin: 10
            anchors.topMargin: 10
            textFormat: Text.RichText
        }
    }
}
