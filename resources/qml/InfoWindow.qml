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
    Layout.margins: 60 * _uiBackend.sizeModifer
    Layout.fillWidth: true
    Layout.fillHeight: true
    transformOrigin: Item.BottomRight
    Layout.alignment: Qt.AlignRight | Qt.AlignBottom

    radius: 10 * _uiBackend.sizeModifer

    ScrollView
    {
        anchors.fill: parent
        anchors.rightMargin: 15
        anchors.topMargin: 10
        clip: true

        TextArea {
            readOnly: true
            id: infoText
            text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 5.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n
                <html>

               <body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n
                <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">
                <h1>Welcome to DoW Stats 2</h1></p>\n
                <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">
              <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">
                <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:600; color:#333333; background-color:#ffffff;\">Standard game settings for sending statistics:</span></p>\n
                <ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">1v1 Game rules: Annihilate, Control Area, Game Timer (optional), Take and Hold.</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">2v2, 3v3, 4v4 Game rules: Annihilate, Control Area (optional), Game Timer (optional), Take and Hold (optional).</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Ai difficulty (optional), if there is AI in the game, statistics are not sent.</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Starting resources: Standard</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Lock teams: Yes</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Enable cheats: No</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Starting location: Optional</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Game speed: Normal</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Resource sharing: No</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Resource Rate: Standard</span></li>\n
                <li style=\" font-size:12pt; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Game duration is more than 30 seconds</span></li></ul>\n
                <p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:x-large; font-weight:600; color:#333333;\"><br /></p>\n
                <p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:600; color:#333333; background-color:#ffffff;\">Developers</span></p>\n
                <ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">
                <li style=\" font-size:x-large; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Dubina_xdd - DoW Stats 2 client main developer </span></li>\n
                <li style=\" font-size:x-large; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">Anibus - dowstats web site main developer, DoW Stats 2 client developer </span></li>\n
                <li style=\" font-size:x-large; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">New - first client (SSStats) main developer</span></li>\n
                <li style=\" font-size:x-large; color:#333333; background-color:#ffffff;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large;\">With participation: elamaunt, TranspersonalPsy, Drobor, NCTribit, ZADGE, Nascent</span></li></ul>
                <br>
                <center><h2>Players are divided into the following ranks by Solo MMR</h2></center><br>
                <table align=\"center\" style=\"margin: 0px auto;\">
                <tr>
                    <td><center>Calibrate</center></td>
                    <td><center>Division VII</center></td>
                    <td><center>Division VI</center></td>
                    <td><center>Division V</center></td>
                    <td><center>Division IV</center></td>
                    <td><center>Division III</center></td>
                    <td><center>Division II</center></td>
                    <td><center>Division I</center></td>
                </tr>
                <tr>
                    <td><center>-</center></td>
                    <td><center>0 - 1399</center></td>
                    <td><center>1400 - 1549</center></td>
                    <td><center>1550 - 1649</center></td>
                    <td><center>1650 - 1799</center></td>
                    <td><center>1800 - 1999</center></td>
                    <td><center>2000 - 2249</center></td>
                    <td><center>2250+</center></td>
                </tr>
                <tr>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/calibrate_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank0_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank1_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank2_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank3_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank4_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank5_60.png\" width=\"70\" height=\"70\"></td>
                    <td style=\" padding: 0px 10px 0px 10px; width: 120px;\" ><img style=\" margin: 0px auto; \" src=\"qrc:/images/resources/images/rank6_60.png\" width=\"70\" height=\"70\"></td>
                </tr>
                </table>
                <br>
                <br>
                <center>To receive a rank medal, a player must play 10 calibration games according to 1v1 rules.</center>
                <br>
                <br>
                </body></html>
                "
            //anchors.fill: parent
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            anchors.margins: 10 * _uiBackend.sizeModifer


        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;annotation:"1 //;;//  //;;//  //;;// <!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#333333;\">Welcome to SS Stats 2</span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#333333;\">Requirements for collecting statistics</span></p>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">In order for the current steam account to be displayed correctly in the client, you need to make it public. In your Steam profile, open the privacy settings menu Edit Profile -&gt; Privacy Settings. Set the following settings: My profile: Public, Game details: Public.</span></li></ul>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#333333;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#333333;\">Standart game settings for sending statistic:</span></p>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">Game rules: Annihilate, Control Area, Game Timer (optional), Take and Hold.</span></li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Ai difficulty (optional), if there is AI in the game, statistic are not sent.</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Starting resources: Standard</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Lock teams: Yes</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Enable cheats: No</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Starting location: Optional</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Game speed: Nomal</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Resource sharing: No</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Resource Rate: Standard</li></ul>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#333333;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#333333;\">Recomendatitons</span></p>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">Searching for a player in the lobby does not always work correctly, sometimes players may not be found. If the player is not found, try pressing the highlighted ready button in your profile element, very often this will fix a bug in the virtual memory of the game, and the players are detected by the client.</span></li></ul>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#333333;\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#333333;\">Developers</span></p>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">Dubina_xdd - SS Stats 2 client main developer</span></li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Anibus - dowstats web site main developer, SS Stats 2 client developer </li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">NCTribit - SS Stats 2 client developer</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">New - first client (SSStats) main developer</li>\n<li style=\" font-size:12pt; color:#333333;\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">With participation: elamaunt, TranspersonalPsy, Drobor</li></ul></body></html> //;;// 1633886754";customId:""}
}
##^##*/
