pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal

Button {
    id: control
    checkable: true
    checked: true

    property color checkedColor: Colors.checkableButtonBgChecked
    property color uncheckedColor: Colors.checkableButtonBgUnchecked
    property color checkedTextColor: Colors.checkedText
    property color uncheckedTextColor: Colors.labelText

    contentItem: IconLabel {
        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font.family: control.font.family
        font.pixelSize: 10
        color: control.checked ? control.checkedTextColor : control.uncheckedTextColor
    }

    background: Rectangle {
        property color bgColor: control.checked ? control.checkedColor : control.uncheckedColor
        implicitWidth: 70
        implicitHeight: 20
        border.color: Colors.checkableButtonBorder
        border.width: 1
        radius: 2
        color: control.hovered ? bgColor.lighter(1.2) : bgColor
    }
}
