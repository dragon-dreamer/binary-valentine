import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts

AppLabel {
    property bool applyTopMargin: true
    property int headerLeftMargin: -5

    font.pixelSize: 24
    Layout.bottomMargin: 2
    Layout.topMargin: applyTopMargin ? 15 : 0
    Layout.leftMargin: headerLeftMargin
}
