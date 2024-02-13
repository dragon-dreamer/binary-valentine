pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    required property string text
    property alias font: link.font
    property alias elide: link.elide

    signal linkActivated()

    width: childrenRect.width
    height: childrenRect.height

    Label {
        id: link
        textFormat: Text.StyledText
        property color hoverLinkColor: Qt.darker(link.linkColor, 2.0)
        Component.onCompleted: { hoverLinkColor = hoverLinkColor; }
        onLinkActivated: parent.linkActivated()
        activeFocusOnTab: true
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Return
                || event.key === Qt.Key_Space) {
                event.accepted = true;
                parent.linkActivated();
            }
        }

        text: "<a href='#link'>" + parent.text + "</a>"
        MouseArea {
            anchors.fill: parent
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            acceptedButtons: Qt.NoButton
        }

        states: State {
            name: "hover"
            when: link.hoveredLink || link.activeFocus
            PropertyChanges {
                id: linkChange
                target: link
                linkColor: hoverLinkColor
            }
        }

        transitions: Transition {
            ColorAnimation { duration: 100 }
        }
    }
}
