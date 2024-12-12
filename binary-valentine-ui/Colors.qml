import QtQuick

pragma Singleton

QtObject {
    readonly property color background: "#ffffff"
    readonly property color labelText: "#000000"
    readonly property color labelTextDisabled: "grey"
    readonly property color descriptionLabelText: "#808080"
    readonly property color checkedText: "#ffffff"
    readonly property color invalidValueText: "red"
    readonly property color labelTextReportDescription: "#505050"

    readonly property string infoLevelText: "#107010"
    readonly property string warningLevelText: "#b0b000"
    readonly property string errorLevelText: "#806000"
    readonly property string criticalLevelText: "#f00000"
    readonly property string ruleLevelCatText: "#a0a0a0"

    readonly property color checkableButtonBorder: "#303030"
    readonly property color checkableButtonBgChecked: "green"
    readonly property color checkableButtonBgUnchecked: "#e0e0e0"

    readonly property color tabButtonBgHover: "#eeeeee"
    readonly property color tabButtonBg: "white"
    readonly property color tabButtonBorder: "white"
    readonly property color tabButtonBorderSelected: "darkgrey"
    readonly property color tabButtonBorderSelectedPressed: "lightgrey"

    readonly property color fileDropAreaActive: "#bebe70"

    readonly property color treeItemBgCurrent: "#e0e0ff"
    readonly property color treeItemBgHovered: "#e7e7ff"

    readonly property color frameBorder: "#c0c0c0"

    readonly property color listViewHighlightBorder: "#a0a0a0"
    readonly property color listViewHighlightBg: "#d0d0ff"

    readonly property color buttonLevelCriticalChecked: "#e8ffb6"
    readonly property color buttonLevelErrorChecked: "#e8ffb6"
    readonly property color buttonLevelWarningChecked: "#e8ffb6"
    readonly property color buttonLevelInfoChecked: "#e8ffb6"
    readonly property color buttonCategoryChecked: "#8090ff"
    readonly property color buttonLevelIconColorUnchecked: "grey"
    readonly property color buttonLevelLabelTextUnchecked: "#505050"
}
