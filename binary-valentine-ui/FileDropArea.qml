import QtQuick

import "UrlHelper.js" as UrlHelper

Item {
    id: root
    signal filesDropped(list<string> paths)

    DropArea {
        anchors.fill: parent

        onEntered: (dragEvent) => logic.onDragEnter(dragEvent)

        onExited: dropRect.visible = false;

        onDropped: (dragEvent) => logic.onDropped(dragEvent)
    }

    Rectangle {
        id: dropRect
        anchors.fill: parent
        opacity: 0.5
        color: Colors.fileDropAreaActive
        visible: false
    }

    QtObject {
        id: logic

        function onDragEnter(dragEvent: DragEvent): void {
            if (!dragEvent.hasUrls) {
                return;
            }

            const helper = new UrlHelper.UrlHelper(dragEvent.urls);
            if (!helper.areLocalFiles()) {
                return;
            }

            dropRect.visible = true;
        }

        function onDropped(dragEvent: DragEvent): void {
            dropRect.visible = false;

            if (!dragEvent.hasUrls) {
                return;
            }

            const helper = new UrlHelper.UrlHelper(dragEvent.urls);
            if (!helper.areLocalFiles()) {
                return;
            }

            root.filesDropped(helper.getLocalFilePaths());
        }
    }
}
