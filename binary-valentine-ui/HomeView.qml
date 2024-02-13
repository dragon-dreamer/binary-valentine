import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Universal

import "UrlHelper.js" as UrlHelper

import BinaryValentineNative

Pane {
    id: root

    signal fileScanRequested(list<string> filePaths)
    signal newProjectRequested()
    signal existingProjectOpened(string filePath)

    ColumnLayout {
        anchors.centerIn: parent

        RowLayout {
            AppLabel {
                id: headerLabel
                text: qsTr("Welcome to %1!").arg(VersionInfo.toolName)
                font.bold: true
                font.pixelSize: 36
            }
            Image {
                source: "res/logo-small.png"
            }

            Layout.bottomMargin: 20
        }

        Link {
            text: qsTr("Scan executable file")
            onLinkActivated: executableFileDialog.open()
        }
        Link {
            text: qsTr("Scan folder")
            onLinkActivated: executableFolderDialog.open()
        }
        Link {
            text: qsTr("Create new project")
            onLinkActivated: root.newProjectRequested()
        }
        Link {
            text: qsTr("Open existing project")
            onLinkActivated: openProjectDialog.open()
        }

        AppLabel {
            Layout.topMargin: 20
            Layout.maximumWidth: 400
            wrapMode: Label.WordWrap
            text: qsTr("You can drag and drop files ans folders on this window to scan them with default settings.")
        }
    }

    FileDropArea {
        anchors.fill: parent
        onFilesDropped: (paths) => logic.tryScanPaths(paths)
    }

    ScanFileDialog {
        id: executableFileDialog
        onAccepted: logic.triggerFileScanSignal(selectedFiles)
    }
    ScanFolderDialog {
        id: executableFolderDialog
        onAccepted: logic.triggerFileScanSignal(selectedFolder)
    }
    OpenProjectDialog {
        id: openProjectDialog
        onAccepted: logic.triggerOpenProjectSignal(selectedFile)
    }

    QtObject {
        id: logic

        function triggerFileScanSignal(urls: list<url>): void {
            const helper = new UrlHelper.UrlHelper(urls);
            if (!helper.areLocalFiles()) {
                return;
            }

            root.fileScanRequested(helper.getLocalFilePaths());
        }

        function triggerOpenProjectSignal(url: url): void {
            const helper = new UrlHelper.UrlHelper(url);
            if (!helper.areLocalFiles()) {
                return;
            }

            root.existingProjectOpened(helper.getLocalFilePaths()[0]);
        }

        function tryScanPaths(paths: list<string>): void {
            if (paths.length === 1) {
                const lowerCasePath = paths[0].toLowerCase();
                if (lowerCasePath.endsWith('.xml') || lowerCasePath.endsWith('.bvproj')) {
                    root.existingProjectOpened(paths[0]);
                    return;
                }
            }

            root.fileScanRequested(paths);
        }
    }
}
