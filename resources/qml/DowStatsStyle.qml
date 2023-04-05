pragma Singleton
import QtQuick 2.0
import Theme 1.0

QtObject {

    property var currentTheme: Theme.Light

    property color backgroundColor: "#FFFFFF"
    property color alternateBackgroundColor: "#eaeaea"
    property color itemColor: "#c8c8c8"
    property color highlightItemColor: "#DCDCDC"
    property color selectionColor: "#A9A9A9"
    property color textColor: "#26282a"


    function setDarkTheme()
    {
        currentTheme = Theme.Dark;
    }

    function setLightTheme()
    {
        currentTheme = Theme.Light;
    }

    onCurrentThemeChanged:
    {
        if (currentTheme === Theme.Light)
        {
            backgroundColor = "#FFFFFF";
            alternateBackgroundColor = "#eaeaea";
            itemColor = "#c8c8c8";
            highlightItemColor = "#DCDCDC";
            selectionColor = "#c8c8c8";
            textColor = "#1B2631";
        }
        else
        {
            backgroundColor = "#212F3D";
            alternateBackgroundColor = "#34495E";
            itemColor = "#2E4053";//"#85929E";
            highlightItemColor = "#566573";
            selectionColor = "#5D6D7E"//"#2C3E50"//"#ABB2B9";
            textColor = "#F8F9F9";
        }
    }
}
