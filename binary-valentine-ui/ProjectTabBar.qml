import QtQuick
import QtQuick.Controls

Item {
    property alias currentIndex: tabs.currentIndex

    implicitWidth: tabs.implicitWidth
    implicitHeight: tabs.implicitHeight + 2

    function addNewTab(): void {
        const tabButton = Qt.createComponent("ProjectTabButton.qml");
        const obj = tabButton.createObject(tabs);
        tabs.addItem(obj);
        tabs.setCurrentIndex(tabs.count - 1);
    }

    function renameTab(index: int, newName: string, hasAnyChanges: bool): void {
        const item = tabs.itemAt(index) as ProjectTabButton;
        item.tabName = newName;
        item.hasAnyChanges = hasAnyChanges;
    }

    function closeTab(index) {
        tabs.takeItem(index);
    }

    TabBar {
        id: tabs
        y: 5
        x: 5
        width: parent.width - 10

        ProjectTabButton {}
    }

    Rectangle {
        width: parent.width
        anchors.top: tabs.bottom
        anchors.topMargin: -2
        height: 2
        color: Colors.tabButtonBorderSelected
    }
    Rectangle {
        width: tabs.currentItem.width - 4
        x: tabs.currentItem.x - tabs.contentItem.contentX + 7
        anchors.top: tabs.bottom
        anchors.topMargin: -2
        height: 2
        color: Colors.tabButtonBorder
    }
}
