pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import BinaryValentineNative

PropertyView {
    id: root
    required property ScanRunner node

    fillHeight: true

    HeaderLabel {
        applyTopMargin: false
        text: qsTr("Executables analysis")
    }

    Flow {
        Layout.fillWidth: true
        spacing: 5
        Button {
            enabled: !root.node.analysisInProgress
            text: qsTr("Start executables analysis")
            onClicked: logic.startAnalysis()
        }

        DelayButton {
            enabled: root.node.analysisInProgress && !logic.analysisStopRequested
            delay: 1200
            text: qsTr("Click and hold to stop analysis")
            onActivated: logic.stopAnalysis()
        }
    }

    Label {
        Layout.topMargin: 20
        text: qsTr("Total processed files: <b>%1</b>, total analyzed files: <b>%2</b>").arg(
                  logic.currentProgress.totalRead).arg(logic.currentProgress.totalAnalyzed)
    }

    RowLayout {
        Layout.topMargin: 20
        visible: root.node.analysisInProgress
        spacing: 20

        BusyIndicator {
            running: root.node.analysisInProgress
        }

        Label {
            Layout.fillWidth: true
            elide: Text.ElideLeft
            text: logic.currentProgress.currentAnalyzedTargetPath
        }
    }

    ListModel {
        id: analyzedFilesListModel
    }

    ListModel {
        id: analyzedFileReportsListModel
    }

    Label {
        Layout.topMargin: 10
        text: qsTr("<b>Reports</b>")
        font.pixelSize: 18
    }

    Flow {
        Layout.fillWidth: true
        spacing: 5

        CheckableButton {
            text: qsTr("FATAL")
            checkedColor: Colors.buttonLevelCriticalChecked
            checked: root.node.reportLevels & RuleSelector.ReportLevelCritical
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelCritical, checked)
        }
        CheckableButton {
            text: qsTr("ERROR")
            checkedColor: Colors.buttonLevelErrorChecked
            checked: root.node.reportLevels & RuleSelector.ReportLevelError
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelError, checked)
        }
        CheckableButton {
            text: qsTr("WARN")
            checkedColor: Colors.buttonLevelWarningChecked
            checkedTextColor: Colors.labelText
            checked: root.node.reportLevels & RuleSelector.ReportLevelWarning
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelWarning, checked)
        }
        CheckableButton {
            text: qsTr("INFO")
            checkedColor: Colors.buttonLevelInfoChecked
            checked: root.node.reportLevels & RuleSelector.ReportLevelInfo
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelInfo, checked)
        }

        Rectangle {
            width: 3
            height: 22
            color: Colors.listViewHighlightBorder
        }

        CheckableButton {
            text: qsTr("Security")
            checkedColor: Colors.buttonCategoryChecked
            checked: root.node.reportCategories & RuleSelector.ReportCategorySecurity
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategorySecurity, checked)
        }
        CheckableButton {
            text: qsTr("Config")
            checkedColor: Colors.buttonCategoryChecked
            checked: root.node.reportCategories & RuleSelector.ReportCategoryConfiguration
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryConfiguration, checked)
        }
        CheckableButton {
            text: qsTr("Optimize")
            checkedColor: Colors.buttonCategoryChecked
            checked: root.node.reportCategories & RuleSelector.ReportCategoryOptimization
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryOptimization, checked)
        }
        CheckableButton {
            text: qsTr("System")
            checkedColor: Colors.buttonCategoryChecked
            checked: root.node.reportCategories & RuleSelector.ReportCategorySystem
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategorySystem, checked)
        }
        CheckableButton {
            text: qsTr("Format")
            checkedColor: Colors.buttonCategoryChecked
            checked: root.node.reportCategories & RuleSelector.ReportCategoryFormat
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryFormat, checked)
        }
    }

    Frame {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.maximumHeight: 100

        background: Rectangle {
            color: "transparent"
            border.color: Colors.frameBorder
            radius: 2
        }

        ScrollView  {
            width: parent.width
            height: parent.height
            contentHeight: analyzedFilesList.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ListView {
                model: analyzedFilesListModel
                id: analyzedFilesList
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                clip: true
                pixelAligned: true
                highlightMoveDuration: 0
                highlightResizeDuration: 0
                spacing: 5
                activeFocusOnTab: true

                highlight: Rectangle {
                    color: Colors.listViewHighlightBg;
                    radius: 2
                }

                delegate: MouseArea {
                    id: fileRow
                    required property var model
                    required property int index
                    width: ListView.view.width - 20
                    height: childrenRect.height

                    RowLayout {
                        width: parent.width
                        height: childrenRect.height

                        Image {
                            id: fileIcon
                            width: 16
                            height: 16
                            source: "res/level-" + logic.toLevel(fileRow.model.level) + ".png"
                        }

                        Label {
                            text: fileRow.model.path
                            Layout.fillWidth: true
                            elide: Label.ElideLeft
                        }
                    }

                    onClicked: {
                        root.node.selectedFileIndex = fileRow.model.fileIndex;
                        analyzedFilesList.currentIndex = fileRow.index;
                    }
                }

                onCurrentItemChanged: {
                    if (currentItem !== null) {
                        logic.loadReport();
                    }
                }
            }
        }
    }

    Frame {
        Layout.fillWidth: true
        Layout.fillHeight: true

        background: Rectangle {
            color: "transparent"
            border.color: Colors.frameBorder
            radius: 2
        }
        ScrollView  {
            width: parent.width
            height: parent.height
            contentHeight: reportsList.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ListView {
                model: analyzedFileReportsListModel
                id: reportsList
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                clip: true
                pixelAligned: true
                highlightMoveDuration: 0
                highlightResizeDuration: 0
                spacing: 5

                highlight: Rectangle {
                    color: Colors.listViewHighlightBg
                    radius: 2
                }

                delegate: MouseArea {
                    id: reportRow

                    required property var model
                    required property int index
                    property bool isCurrentItem: ListView.isCurrentItem
                    property bool isCommonReport: !model.id

                    width: ListView.view.width - 20
                    height: childrenRect.height

                    Column {
                        width: parent.width

                        RowLayout {
                            width: parent.width
                            Image {
                                id: reportIcon
                                width: 16
                                height: 16
                                source: "res/level-" + reportRow.model.level + ".png"
                            }
                            Label {
                                Layout.fillWidth: true
                                text: reportRow.isCommonReport ?
                                          logic.getCommonReportName(reportRow.model.level) :
                                          (reportRow.model.id + ": " + reportRow.model.name)
                                font.bold: reportRow.isCurrentItem
                                elide: Label.ElideRight
                                textFormat: Label.PlainText
                            }
                        }

                        Label {
                            x: 21
                            visible: reportRow.isCurrentItem
                            width: parent.width - x
                            text: visible ? reportRow.model.message : ""
                            wrapMode: visible ? Label.WordWrap : Label.NoWrap
                            color: Colors.labelTextReportDescription
                            font.pixelSize: 14
                            textFormat: Label.PlainText
                        }

                        Link {
                            x: 21
                            visible: reportRow.isCurrentItem && !reportRow.isCommonReport
                            width: parent.width - x
                            text: visible ? "<a href='#'>View more details online...</a>" : ""
                            elide: visible ? Label.ElideRight : Label.ElideNone
                            font.pixelSize: 14
                            onLinkActivated: {
                                Qt.openUrlExternally(VersionInfo.getRuleInfoUrl(reportRow.model.id));
                            }
                        }
                    }

                    onClicked: {
                        reportsList.currentIndex = reportRow.index;
                    }
                }
            }
        }
    }

    Timer {
        running: root.node.analysisInProgress
        repeat: true
        interval: 250
        onTriggered: logic.updateProgress()
    }

    Component.onCompleted: {
        root.node.analysisInProgressChanged.connect(logic.analysisInProgressChanged);
        root.node.reportLevelsChanged.connect(logic.reloadReport);
        root.node.reportCategoriesChanged.connect(logic.reloadReport);

        logic.reloadReport();
    }

    Component.onDestruction: {
        root.node.analysisInProgressChanged.disconnect(logic.analysisInProgressChanged);
        root.node.reportLevelsChanged.disconnect(logic.reloadReport);
        root.node.reportCategoriesChanged.disconnect(logic.reloadReport);
    }

    QtObject {
        id: logic

        property bool analysisStopRequested: false
        property var currentProgress: root.node.getAnalysisProgress()

        function toLevel(level: int) {
            if (level === 0) {
                return "progress";
            }

            if (level === -1) {
                return "no";
            }

            return level - 1;
        }

        function checkReportLevel(level, checked: bool): void {
            if (checked) {
                root.node.reportLevels |= level;
            } else {
                root.node.reportLevels &= ~level;
            }
        }

        function checkReportCategory(category, checked: bool): void {
            if (checked) {
                root.node.reportCategories |= category;
            } else {
                root.node.reportCategories &= ~category;
            }
        }

        function getCommonReportName(level): string {
            switch (level) {
            case BinaryValentineContext.ReportLevelCritical:
                return qsTr("Critical application error");
            case BinaryValentineContext.ReportLevelError:
                return qsTr("Application error");
            case BinaryValentineContext.ReportLevelWarning:
                return qsTr("Application warning");
            default:
                return qsTr("Informational message");
            }
        }

        function analysisInProgressChanged(): void {
            if (!root.node.analysisInProgress) {
                updateProgress();
                reloadReport();
            }
        }

        function prepareFilesModel(): void {
            analyzedFilesListModel.clear();
            analyzedFilesListModel.append(
                        {
                            "path": qsTr("General issues"),
                            "fileIndex": 0,
                            "level": -1
                        });
        }

        function startAnalysis(): void {
            prepareFilesModel();
            updateProgress();
            root.node.startAnalysis();
            analysisStopRequested = false;
        }

        function stopAnalysis(): void {
            analysisStopRequested = true;
            root.node.requestAnalysisStop();
        }

        function reloadAllFiles(): void {
            prepareFilesModel();
            const allFiles = root.node.getAllFiles();
            var selectedModelIndex = 0;
            const selectedFileIndex = root.node.selectedFileIndex;
            for (var i = 0, len = allFiles.length; i !== len; ++i) {
                const obj = allFiles[i];
                analyzedFilesListModel.append(obj);
                if (obj.fileIndex === selectedFileIndex) {
                    selectedModelIndex = i + 1;
                }
            }
            analyzedFilesList.currentIndex = selectedModelIndex;
        }

        function updateProgress(): void {
            currentProgress = root.node.getAnalysisProgress();
            for (var i = 0, len = currentProgress.newFiles.length; i !== len; ++i) {
                const obj = currentProgress.newFiles[i];
                analyzedFilesListModel.insert(obj.index, obj);
            }
        }

        function reloadReport(): void {
            if (!root.node.analysisInProgress) {
                reloadAllFiles();
            }
            loadReport();
        }

        function loadReport(): void {
            analyzedFileReportsListModel.clear();
            const report = root.node.getFileReport();
            for (var i = 0, len = report.length; i !== len; ++i) {
                analyzedFileReportsListModel.append(report[i]);
            }
        }
    }
}
