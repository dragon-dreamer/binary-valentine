import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Label {
    property Item target

    Layout.fillWidth: true
    Layout.leftMargin: target && target.hasOwnProperty("contentItem") ? target.contentItem.leftPadding : 0
    wrapMode: Label.WordWrap
    color: Colors.descriptionLabelText
    font.pixelSize: 14
}
