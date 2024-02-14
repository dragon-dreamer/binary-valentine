pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal

TabButton {
    id: tabButton

    property bool isSelected:
        TabBar.tabBar === null ? 0 : TabBar.index == TabBar.tabBar.currentIndex

    background: Item {
        Rectangle {
            anchors.fill: parent
            border.color: !tabButton.isSelected ? Colors.tabButtonBorder :
                                                  (tabButton.down
                                                   ? Colors.tabButtonBorderSelectedPressed
                                                   : Colors.tabButtonBorderSelected)
            border.width: 2
            color: tabButton.hovered && !tabButton.isSelected ? Colors.tabButtonBgHover : Colors.tabButtonBg
        }
        Rectangle {
            width: parent.width - 4
            x: 2
            anchors.bottom: parent.bottom
            color: Colors.tabButtonBorder
            height: 2
            visible: tabButton.isSelected
        }
        Rectangle {
            width: parent.width
            anchors.bottom: parent.bottom
            color: Colors.tabButtonBorderSelected
            height: 2
            visible: !tabButton.isSelected
        }
    }
}
