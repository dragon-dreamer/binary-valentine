pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs
import BinaryValentineNative

import "UrlHelper.js" as UrlHelper

PropertyView {
    id: root
    required property ProjectSettings node

    HeaderLabel {
        applyTopMargin: false
        text: qsTr("Root path")
    }
    RowLayout {
        TextField {
            id: rootPathText
            Layout.fillWidth: true
            text: root.node.rootPath
            placeholderText: qsTr("(empty - use current directory)")
            onTextEdited: root.node.rootPath = text
        }
        Button {
            text: qsTr("...")
            onClicked: {
                rootPathDialog.currentFolder = "file:///" + rootPathText.text
                rootPathDialog.open()
            }
        }
    }
    DescriptionLabel {
        text: qsTr("This path will be used as a root path for all relative paths in the project."
                   + " If absent, a current directory path will be used as a root path.")
    }


    HeaderLabel {
        text: qsTr("Combined analysis")
    }
    Switch {
        id: combinedAnalysis
        checked: root.node.combinedAnalysis
        leftPadding: 0
        text: qsTr("Perform combined analysis")
        onToggled: root.node.combinedAnalysis = checked
    }
    DescriptionLabel {
        target: combinedAnalysis
        text: qsTr("Combined rules detect issues across all executables which are being scanned by a project. "
                   + "For example, there are rules which detect version information inconsistencies across all analyzed files.")
    }


    HeaderLabel {
        text: qsTr("Analysis threads")
    }
    Switch {
        id: useCustomThreadCount
        checked: root.node.useCustomThreadCount
        leftPadding: 0
        text: qsTr("Set custom thread count")
        onToggled: root.node.useCustomThreadCount = checked
    }
    RowLayout {
        visible: useCustomThreadCount.checked
        Layout.leftMargin: useCustomThreadCount.contentItem.leftPadding

        Label {
            text: qsTr("Analysis thread count")
        }

        SpinBox {
            id: threadCount
            value: root.node.threadCount
            from: 1
            to: 99
            editable: true
            onValueChanged: root.node.threadCount = value
        }
    }
    DescriptionLabel {
        target: useCustomThreadCount
        visible: useCustomThreadCount.checked
        text: qsTr("This value sets the number of analysis threads."
                   + " By default, a number of threads which corresponds to the number of processor cores is used.")
    }


    HeaderLabel {
        text: qsTr("File analysis limits")
    }
    ComboBox {
        id: concurrentLimitType
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "value"
        model: [
            { value: ProjectSettings.AnalysisLimitTypeDefault, text: qsTr("Apply default limit") },
            { value: ProjectSettings.AnalysisLimitTypeSize, text: qsTr("Limit loaded files size") },
            { value: ProjectSettings.AnalysisLimitTypeFileCount, text: qsTr("Limit number of loaded files") }
        ]
        Component.onCompleted: currentIndex = indexOfValue(root.node.analysisLimitType)
        onActivated: root.node.analysisLimitType = currentValue
    }
    DescriptionLabel {
        target: concurrentLimitType
        visible: concurrentLimitType.currentValue === ProjectSettings.AnalysisLimitTypeDefault
        Layout.leftMargin: 0
        text: qsTr("By default, Binary Valentine loads up to 1 gigabyte of data into memory.")
    }
    RowLayout {
        visible: concurrentLimitType.currentValue === ProjectSettings.AnalysisLimitTypeSize

        Label {
            text: qsTr("Maximum size of loaded files")
        }

        SpinBox {
            value: root.node.totalFileSizeLimit
            from: 32
            to: 65536
            stepSize: 32
            editable: true
            onValueChanged: root.node.totalFileSizeLimit = value
            textFromValue: logic.fileSizeTextFromValue
            valueFromText: logic.fileSizeValueFromText
        }
    }
    DescriptionLabel {
        target: concurrentLimitType
        visible: concurrentLimitType.currentValue === ProjectSettings.AnalysisLimitTypeSize
        Layout.leftMargin: 0
        text: qsTr("Maximum amount of memory to use for preloaded files pending analysis.")
    }
    RowLayout {
        visible: concurrentLimitType.currentValue === ProjectSettings.AnalysisLimitTypeFileCount

        Label {
            text: qsTr("Maximum number of loaded files")
        }

        SpinBox {
            value: root.node.concurrentFilesLimit
            from: 1
            to: 100
            editable: true
            onValueChanged: root.node.concurrentFilesLimit = value
        }
    }
    DescriptionLabel {
        target: concurrentLimitType
        visible: concurrentLimitType.currentValue === ProjectSettings.AnalysisLimitTypeFileCount
        Layout.leftMargin: 0
        text: qsTr("Maximum number of preloaded files pending analysis.")
    }

    Item {
        Layout.fillHeight: true
    }

    QtObject {
        id: logic
        function fileSizeTextFromValue(value, locale): string {
            var valNumber = Number(value);
            var postfix = qsTr(" MB");
            return valNumber.toLocaleString(locale, 'f', 0) + postfix;
        }

        function fileSizeValueFromText(text, locale) {
            if (text.endsWith(" MB")) {
                text = text.substr(0, text.length - 3);
            }

            return Number.fromLocaleString(locale, text);
        }
    }

    FolderDialog {
        id: rootPathDialog
        acceptLabel: qsTr("Select")
        rejectLabel: qsTr("Cancel")
        options: FolderDialog.ReadOnly
        onAccepted: function() {
            const urlHelper = new UrlHelper.UrlHelper(currentFolder);
            if (!urlHelper.areLocalFiles()) {
                return;
            }

            node.rootPath = urlHelper.getLocalFilePaths()[0];
        }
    }
}
