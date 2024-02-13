import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    default property alias contents : nestedItems.data
    property bool fillHeight: false

    id: scrollView
    Layout.fillWidth: true
    Layout.fillHeight: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height
        contentWidth: parent.width
        contentHeight: nestedItems.height
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds
        clip: true
        pixelAligned: true

        ColumnLayout {
            id: nestedItems

            x: 10
            width: parent.width - 20
            height: scrollView.fillHeight ? scrollView.height : undefined
            spacing: 3
        }
    }
}
