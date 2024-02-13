import QtQuick

DynamicTabButton {
    property string tabName : qsTr("Home")
    property bool hasAnyChanges: false

    text: hasAnyChanges ? ("<b>" + tabName + "</b>") : tabName
    width: 300
}
