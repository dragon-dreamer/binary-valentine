pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal

TabButton {
    id: tabButton

    signal tabCloseRequested()

    property bool isSelected:
        TabBar.tabBar === null ? 0 : TabBar.index == TabBar.tabBar.currentIndex

    contentItem: Rectangle {
        implicitHeight: 32

        color: "transparent"
        border.color: "transparent"

        Label {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: tabButton.text
            font.pixelSize: 24
            font.weight: 300
            color: tabButton.checked ? Colors.labelText : Colors.labelTextDisabled
            elide: Label.ElideLeft
        }

        Button {
            id: closeButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            icon.source: closeButton.down ? "res/close-down.png" :
                                            (closeButton.hovered || closeButton.visualFocus
                                             ? "res/close.png" : "res/close-inactive.png")
            icon.color: "transparent"
            icon.width: 24
            icon.height: 24
            width: 24
            height: 24
            spacing: 0
            padding: 0
            implicitWidth: 24
            implicitHeight: 24

            background: Item {
                visible: false
            }

            onClicked: tabButton.tabCloseRequested()
        }
    }

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
