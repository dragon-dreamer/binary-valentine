pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs
import BinaryValentineNative

import "UrlHelper.js" as UrlHelper

PropertyView {
    id: root
    required property ReportOutputs node

    HeaderLabel {
        applyTopMargin: false
        text: qsTr("Report outputs")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("On this page, you can set up the output reports (terminal output, and SARIF or text files)."
                   + " When running a scan via the UI, you will be able to see the progress and"
                   + " issues in the real time.")
    }

    Switch {
        id: terminalOutputSwitch
        Layout.topMargin: 15
        checked: root.node.terminalOutput
        leftPadding: 0
        text: qsTr("Print to terminal")
        onToggled: root.node.terminalOutput = checked
    }
    DescriptionLabel {
        target: terminalOutputSwitch
        text: qsTr("Print analysis report to terminal "
                   + "when running the scan in the console version of %1").arg(VersionInfo.toolName)
    }


    HeaderLabel {
        text: qsTr("Output to files")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("You can specify relative or absolute paths for output report files."
                   + " When using a relative path, it will be relative to the root path specified"
                   + " in the project settings.")
    }
    ListModel {
        id: listModel
        Component.onCompleted: logic.loadOutputFiles(listModel)
    }
    Link {
        text: qsTr("Add new")
        onLinkActivated: logic.addNewOutputFile(listModel)
    }

    Frame {
        Layout.fillWidth: true
        background: Rectangle {
            color: "transparent"
            border.color: Colors.frameBorder
            radius: 2
        }
        ScrollView  {
            id: outputFilesScrollView
            width: parent.width
            contentHeight: outputFilesList.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ListView {
                id: outputFilesList
                model: listModel
                height: 200
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                clip: true
                pixelAligned: true
                highlightMoveDuration: 0
                highlightResizeDuration: 0
                spacing: 5

                delegate: RowLayout {
                    width: ListView.view.width - 20
                    spacing: 0
                    required property var model
                    required property int index

                    ComboBox {
                        id: fileFormatCombo
                        model: [
                            { value: ReportOutputs.FileFormatText,
                                text: qsTr("Plaintext") },
                            { value: ReportOutputs.FileFormatSarif,
                                text: qsTr("SARIF") },
                            { value: ReportOutputs.FileFormatHtml,
                                text: qsTr("HTML") },
                        ]
                        implicitContentWidthPolicy: ComboBox.WidestText
                        textRole: "text"
                        valueRole: "value"
                        Component.onCompleted: currentIndex = indexOfValue(parent.model.fileFormat)
                        onActivated: logic.setOutputFileFormat(parent.index,
                                                               currentValue,
                                                               filePathTextField)
                    }

                    TextField {
                        id: filePathTextField
                        Layout.fillWidth: true
                        Layout.leftMargin: 5
                        text: parent.model.path
                        placeholderText: qsTr("Can not be empty")
                        onTextEdited: logic.setOutputFilePath(parent.index, text)
                    }
                    Button {
                        text: qsTr("...")
                        onClicked: logic.selectOutputFile(listModel, parent.index,
                                                          fileFormatCombo.currentValue,
                                                          filePathTextField)
                    }
                    Button {
                        Layout.leftMargin: 5
                        text: qsTr("Advanced...")
                        visible: fileFormatCombo.currentValue === ReportOutputs.FileFormatHtml
                        onClicked: logic.handleAdvancedOptions(parent.model,
                                                               parent.index,
                                                               filePathTextField);
                    }
                    Button {
                        Layout.leftMargin: 5
                        text: qsTr("Delete")
                        onClicked: logic.deleteOutputFile(listModel, parent.index)
                    }
                }
            }
        }
    }

    Column {
        id: extraOptionsBox
        visible: false
        spacing: 5
        Layout.fillWidth: true
        Layout.topMargin: 15

        Label {
            id: advancedOptionsForReportHeader
            wrapMode: Label.WordWrap
            width: parent.width
        }

        Switch {
            id: useDefaultHtmlTemplateSwitch
            leftPadding: 0
            text: qsTr("Use default HTML report template")
            onToggled: logic.enableDefaultTemplate(checked)
        }

        RowLayout {
            visible: !useDefaultHtmlTemplateSwitch.checked
            width: parent.width
            TextField {
                id: htmlTemplatePathField
                Layout.fillWidth: true
                Layout.leftMargin: 5
                placeholderText: qsTr("Can not be empty")
                onTextEdited: logic.setTemplatePath(text)
            }
            Button {
                text: qsTr("...")
                onClicked: logic.selectTemplatePathFile()
            }
        }
    }

    FileDialog {
        id: saveReportDialog
        acceptLabel: qsTr("Create")
        rejectLabel: qsTr("Cancel")
        fileMode: FileDialog.SaveFile
    }

    FileDialog {
        id: selectReportTemplateDialog
        acceptLabel: qsTr("Select")
        rejectLabel: qsTr("Cancel")
        fileMode: FileDialog.OpenFile
    }

    QtObject {
        id: logic
        property int lastSelectedIndex: -1
        property var lastSelectedModel

        function loadOutputFiles(model): void {
            const filters = root.node.getOutputFiles();
            for (var key in filters) {
                model.append(filters[key]);
            }
        }
        function addNewOutputFile(model): void {
            extraOptionsBox.visible = false;
            lastSelectedModel = undefined;
            model.append(root.node.addNewOutputFile());
        }
        function setOutputFilePath(index, path): void {
            root.node.setOutputFilePath(index, path);
        }
        function setOutputFileFormat(index, fileFormat, pathField): void {
            root.node.setOutputFileFormat(index, fileFormat);
            var path = pathField.text;
            var lastDot = path.lastIndexOf(".");
            if (lastDot === -1) {
                return;
            }
            path = path.substring(0, lastDot);
            if (fileFormat === ReportOutputs.FileFormatText) {
                path += ".txt";
            } else if (fileFormat === ReportOutputs.FileFormatSarif) {
                path += ".json";
            } else if (fileFormat === ReportOutputs.FileFormatHtml) {
                path += ".html";
            } else {
                return;
            }

            setOutputFilePath(index, path);
            pathField.text = path;
        }
        function deleteOutputFile(model, index): void {
            extraOptionsBox.visible = false;
            lastSelectedModel = undefined;
            model.remove(index);
            root.node.deleteOutputFile(index);
        }
        function selectOutputFile(model, index, fileFormat, pathField): void {
            if (fileFormat === ReportOutputs.FileFormatText) {
                saveReportDialog.nameFilters = [
                            qsTr("Plaintext files (*.txt)"),
                            qsTr("All files (*.*)")
                        ];
            } else if (fileFormat === ReportOutputs.FileFormatSarif) {
                saveReportDialog.nameFilters = [
                            qsTr("SARIF files (*.sarif *.json)"),
                            qsTr("All files (*.*)")
                        ];
            } else if (fileFormat === ReportOutputs.FileFormatHtml) {
                saveReportDialog.nameFilters = [
                            qsTr("HTML files (*.htm *.html)"),
                            qsTr("All files (*.*)")
                        ];
            }

            var slotConnection = () => {
                const helper = new UrlHelper.UrlHelper(saveReportDialog.selectedFile);
                if (helper.areLocalFiles()) {
                    var localPaths = helper.getLocalFilePaths();
                    if (localPaths.length === 1) {
                        logic.setOutputFilePath(index, localPaths[0]);
                        pathField.text = localPaths[0];
                    }
                }

                saveReportDialog.accepted.disconnect(slotConnection);
            };
            saveReportDialog.accepted.connect(slotConnection);
            saveReportDialog.open();
        }

        function handleAdvancedOptions(model, index, filePathTextField): void {
            lastSelectedIndex = index;
            lastSelectedModel = model;
            extraOptionsBox.visible = true;
            useDefaultHtmlTemplateSwitch.checked = model.useDefaultReportTemplate;
            htmlTemplatePathField.text = model.reportTemplate;
            advancedOptionsForReportHeader.text = Qt.binding(
                        function() { return qsTr("<b>Advanced options for: </b><i>%1</i>").arg(
                                         filePathTextField.text); });
        }

        function enableDefaultTemplate(enable: bool): void {
            root.node.enableDefaultTemplate(lastSelectedIndex, enable);
            lastSelectedModel.useDefaultReportTemplate = enable;
        }

        function setTemplatePath(path): void {
            root.node.setReportTemplatePath(lastSelectedIndex, path);
            lastSelectedModel.reportTemplate = path;
        }

        function selectTemplatePathFile(): void {
            selectReportTemplateDialog.nameFilters = [
                        qsTr("Report template files (*.tpl)"),
                        qsTr("All files (*.*)")
                    ];

            var slotConnection = () => {
                const helper = new UrlHelper.UrlHelper(selectReportTemplateDialog.selectedFile);
                if (helper.areLocalFiles()) {
                    var localPaths = helper.getLocalFilePaths();
                    if (localPaths.length === 1) {
                        logic.setTemplatePath(localPaths[0]);
                        htmlTemplatePathField.text = localPaths[0];
                    }
                }

                selectReportTemplateDialog.accepted.disconnect(slotConnection);
            };
            selectReportTemplateDialog.accepted.connect(slotConnection);
            selectReportTemplateDialog.open();
        }
    }
}
