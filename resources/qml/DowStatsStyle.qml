pragma Singleton
import QtQuick 2.0
import DowStatsStyle 1.0

QtObject {

    property var currentTheme: Theme.Light

    property color backgroundColor: "#FFFFFF"
    property color alternateBackgroundColor: "#eaeaea"//"#DCDCDC"
    property color itemColor: "#c8c8c8"
    property color selectionColor: "#A9A9A9"
    property color textColor: "#1B2631"

    onCurrentThemeChanged:
    {
        if (currentTheme === Theme.Light)
        {
            backgroundColor = "#FFFFFF";
            alternateBackgroundColor = "#eaeaea";
            itemColor = "#c8c8c8";
            selectionColor = "#c8c8c8";
            textColor = "#1B2631";
        }
        else
        {
            backgroundColor = "#34495E ";
            alternateBackgroundColor = "#212F3D";
            itemColor = "#85929E ";
            selectionColor = "#ABB2B9";
            textColor = "#F8F9F9";
        }
    }
}
