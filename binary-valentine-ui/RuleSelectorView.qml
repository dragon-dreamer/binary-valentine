pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BinaryValentineNative

PropertyView {
    id: root
    required property RuleSelector node

    //TODO: add option to inherit global selector
    //TODO: add option to copy selectors

    HeaderLabel {
        applyTopMargin: false
        text: root.node.isGlobal ? qsTr("Global rule selector") : qsTr("Rule selector")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        visible: root.node.isGlobal
        text: qsTr("This section sets up reports, report levels and categories selection settings for all "
                   + "files and directories in the project, unless they have any overrides.")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        visible: !root.node.isGlobal
        text: qsTr("This section sets up reports, report levels and categories selection settings for "
                   + "the specific path. <b>This selector overrides all options specified in the Global selector</b>.")
    }


    HeaderLabel {
        text: qsTr("Report selection")
    }
    ComboBox {
        id: reportSelectionPolicy
        implicitContentWidthPolicy: ComboBox.WidestText
        textRole: "text"
        valueRole: "value"
        model: [
            { value: RuleSelector.SelectAllReports, text: qsTr("Use all available reports") },
            { value: RuleSelector.ExcludeReports, text: qsTr("Exclude some specific reports") },
            { value: RuleSelector.IncludeReports, text: qsTr("Include only specific reports") }
        ]
        Component.onCompleted: currentIndex = indexOfValue(root.node.reportSelectionPolicy)
        onActivated: root.node.reportSelectionPolicy = currentValue
    }
    ListModel {
        id: listModel
        Component.onCompleted: logic.refreshReports(listModel)
    }
    RowLayout {
        visible: reportSelectionPolicy.currentValue !== RuleSelector.SelectAllReports
        Link {
            text: qsTr("Select all")
            onLinkActivated: logic.selectAllReports(true)
        }
        Link {
            text: qsTr("Select none")
            onLinkActivated: logic.selectAllReports(false)
        }
        Link {
            text: qsTr("Toggle")
            onLinkActivated: logic.selectAllReports(undefined)
        }
        Switch {
            text: qsTr("Group by rules")
            onToggled: logic.groupReportsByRules(checked)
        }
    }

    Frame {
        visible: reportSelectionPolicy.currentValue !== RuleSelector.SelectAllReports
        Layout.fillWidth: true
        background: Rectangle {
            color: "transparent"
            border.color: Colors.frameBorder
            radius: 2
        }
        ScrollView  {
            id: reportsScrollView
            width: parent.width
            contentHeight: reportSelectionList.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ListView {
                id: reportSelectionList
                model: listModel
                height: 200
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                clip: true
                highlight: Rectangle {
                    border.color: Colors.listViewHighlightBorder
                    radius: 3
                }
                activeFocusOnTab: true
                pixelAligned: true
                highlightMoveDuration: 0
                highlightResizeDuration: 0

                delegate: CheckBox {
                    required property var model
                    required property int index

                    id: modelCheckBoxes
                    checked: model.checked
                    text: logic.formatReportUid(model)
                    indicator.width: 15
                    indicator.height: 15
                    activeFocusOnTab: false
                    onToggled: {
                        reportSelectionList.currentIndex = index;
                        root.node.selectReport(model.uid, checked);
                        model.checked = checked;
                    }
                }

                section.delegate: Label {
                    required property string section
                    text: section
                    font.bold: true
                    topPadding: 3
                }
            }
        }
    }
    DescriptionLabel {
        visible: reportSelectionPolicy.currentValue === RuleSelector.SelectAllReports
        Layout.leftMargin: 0
        text: qsTr("Use all available reports for the executable analysis.")
    }
    DescriptionLabel {
        visible: reportSelectionPolicy.currentValue === RuleSelector.ExcludeReports
        Layout.leftMargin: 0
        text: qsTr("Disable some reports for the executable analysis.")
    }
    DescriptionLabel {
        visible: reportSelectionPolicy.currentValue === RuleSelector.IncludeReports
        Layout.leftMargin: 0
        text: qsTr("Include only the specified reports for the executable analysis.")
    }

    DescriptionLabel {
        readonly property int allReportCount: Object.keys(BinaryValentineContext.allAvailableReports).length
        visible: reportSelectionPolicy.currentValue !== RuleSelector.SelectAllReports &&
                 allReportCount === Object.keys(root.node.selectedReports).length
        Layout.leftMargin: 0
        color: Colors.invalidValueText
        font.bold: true
        text: {
            return reportSelectionPolicy.currentValue === RuleSelector.ExcludeReports
                    ? qsTr("You have excluded all available reports. You will get no output when running the analysis.")
                    : qsTr("You have included all available reports. There is no point in having the report filter in this case.")
        }
    }

    DescriptionLabel {
        readonly property int allReportCount: Object.keys(BinaryValentineContext.allAvailableReports).length
        visible: reportSelectionPolicy.currentValue !== RuleSelector.SelectAllReports &&
                 Object.keys(root.node.selectedReports).length === 0
        Layout.leftMargin: 0
        color: Colors.invalidValueText
        font.bold: true
        text: {
            return reportSelectionPolicy.currentValue === RuleSelector.ExcludeReports
                    ? qsTr("You have excluded no reports. There is no point in having the report filter in this case.")
                    : qsTr("You have included no reports. You will get no output when running the analysis.")
        }
    }

    HeaderLabel {
        text: qsTr("Report level selection")
    }
    Flow {
        Layout.fillWidth: true
        CheckBox {
            id: levelInfoCheckbox
            text: qsTr("Info")
            checked: root.node.reportLevels & RuleSelector.ReportLevelInfo
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelInfo, checked)
        }
        CheckBox {
            id: levelWarningCheckbox
            text: qsTr("Warning")
            checked: root.node.reportLevels & RuleSelector.ReportLevelWarning
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelWarning, checked)
        }
        CheckBox {
            id: levelErrorCheckbox
            text: qsTr("Error")
            checked: root.node.reportLevels & RuleSelector.ReportLevelError
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelError, checked)
        }
        CheckBox {
            id: levelCriticalCheckbox
            text: qsTr("Critical")
            checked: root.node.reportLevels & RuleSelector.ReportLevelCritical
            onToggled: logic.checkReportLevel(RuleSelector.ReportLevelCritical, checked)
        }
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("Only reports with the enabled levels will be included in the output.")
    }
    DescriptionLabel {
        visible: !levelInfoCheckbox.checked &&
                 !levelWarningCheckbox.checked &&
                 !levelErrorCheckbox.checked &&
                 !levelCriticalCheckbox.checked
        Layout.leftMargin: 0
        color: Colors.invalidValueText
        font.bold: true
        text: qsTr("You have selected no report levels. You will get no output when running the analysis.")
    }


    HeaderLabel {
        text: qsTr("Report category selection")
    }
    Flow {
        Layout.fillWidth: true
        CheckBox {
            id: categorySystemCheckbox
            text: qsTr("System")
            checked: root.node.reportCategories & RuleSelector.ReportCategorySystem
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategorySystem, checked)
        }
        CheckBox {
            id: categoryOptimizationCheckbox
            text: qsTr("Optimization")
            checked: root.node.reportCategories & RuleSelector.ReportCategoryOptimization
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryOptimization, checked)
        }
        CheckBox {
            id: categorySecurityCheckbox
            text: qsTr("Security")
            checked: root.node.reportCategories & RuleSelector.ReportCategorySecurity
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategorySecurity, checked)
        }
        CheckBox {
            id: categoryConfigurationCheckbox
            text: qsTr("Configuration")
            checked: root.node.reportCategories & RuleSelector.ReportCategoryConfiguration
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryConfiguration, checked)
        }
        CheckBox {
            id: categoryFormatCheckbox
            text: qsTr("Format")
            checked: root.node.reportCategories & RuleSelector.ReportCategoryFormat
            onToggled: logic.checkReportCategory(RuleSelector.ReportCategoryFormat, checked)
        }
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("Only the reports with enabled categories will be included in the output.")
    }
    DescriptionLabel {
        visible: !categorySystemCheckbox.checked &&
                 !categoryOptimizationCheckbox.checked &&
                 !categorySecurityCheckbox.checked &&
                 !categoryConfigurationCheckbox.checked &&
                 !categoryFormatCheckbox.checked
        Layout.leftMargin: 0
        color: Colors.invalidValueText
        font.bold: true
        text: qsTr("You have selected no report categories. You will get no output when running the analysis.")
    }


    HeaderLabel {
        text: qsTr("Custom report filters")
    }
    ComboBox {
        id: customReportRuleComboBox
        model: ListModel {
            id: customReportRuleComboModel
        }
        Layout.fillWidth: true
        textRole: "text"
        valueRole: "uid"
        popup.contentItem.ScrollIndicator.vertical.active: false
        Component.onCompleted: {
            logic.fillAllReportsWithDescriptionArgs(customReportRuleComboModel);
            currentIndex = 0;
        }
    }
    Binding {
        target: customReportRuleComboBox.popup.contentItem.ScrollBar
        property: "vertical"
        value: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }
    }
    Button {
        text: qsTr("Add report filter")
        onClicked: root.node.addReportFilter(customReportRuleComboBox.currentValue)
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("Custom report filters allow to set up the fine-grained report filtering by report output text using regular expressions."
                   + " Only the applicable rules which have at least one argument in their descriptions are included in the list.")
    }


    Button {
        visible: !root.node.isGlobal
        Layout.topMargin: 15
        Layout.alignment: Qt.AlignBottom | Qt.AlignRight
        text: qsTr("Delete rule selector")
        onClicked: root.node.deleteSelector()
    }


    QtObject {
        id: logic

        readonly property string infoText: qsTr("INFO, ")
        readonly property string warnText: qsTr("WARN, ")
        readonly property string errText: qsTr("ERR, ")
        readonly property string fatalText: qsTr("FATAL, ")

        function formatReportUidImpl(model, color: string, levelText: string): string {
            return "<font color='" + color + "'>" + model.uid + "</font> "
                    + model.name
                    + " <font color='" + Colors.ruleLevelCatText + "'>("
                    + levelText + model.category + ")</font>";
        }

        function formatReportUid(model) {
            switch (model.level) {
                case BinaryValentineContext.ReportLevelInfo:
                    return formatReportUidImpl(model, Colors.infoLevelText, infoText);
                case BinaryValentineContext.ReportLevelWarning:
                    return formatReportUidImpl(model, Colors.warningLevelText, warnText);
                case BinaryValentineContext.ReportLevelError:
                    return formatReportUidImpl(model, Colors.errorLevelText, errText);
                case BinaryValentineContext.ReportLevelCritical:
                    return formatReportUidImpl(model, Colors.criticalLevelText, fatalText);
            }
        }

        function selectAllReports(action): void {
            if (action === true) {
                root.node.selectAllReports(true);
            } else if (action === false) {
                root.node.selectAllReports(false);
            } else {
                root.node.toggleAllReports();
            }

            for (var i = 0, c = listModel.count; i !== c; ++i) {
                const obj = listModel.get(i);
                if (action === true) {
                    obj.checked = true;
                } else if (action === false) {
                    obj.checked = false;
                } else {
                    obj.checked = !obj.checked;
                }
            }
        }

        function groupReportsByRules(group): void {
            listModel.clear();

            if (!group) {
                reportSelectionList.section.property = "undefined";
                refreshReports(listModel);
                return;
            }

            reportSelectionList.section.property = "ruleName";
            refreshReports(listModel, (l, r) => {
                               var result = l.ruleName.localeCompare(r.ruleName);
                               if (result !== 0) {
                                   return result;
                               }
                               return l.uid.localeCompare(r.uid);
                           });
        }

        function refreshReports(listModel, sortFunction): void {
            const reports = BinaryValentineContext.allAvailableReports;
            const selectedReports = root.node.selectedReports;
            var reportData = [];
            for (var uid in reports) {
                var report = reports[uid];
                reportData.push({
                                    "uid": uid,
                                    "name": report.name,
                                    "level": report.level,
                                    "category": report.categoryText,
                                    "checked": uid in selectedReports,
                                    "ruleName": report.ruleName
                                });
            }

            if (sortFunction !== undefined) {
                reportData.sort(sortFunction);
            }

            listModel.clear();
            for (var i in reportData) {
                listModel.append(reportData[i]);
            }
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

        function fillAllReportsWithDescriptionArgs(textModel): void {
            const reports = BinaryValentineContext.allAvailableReportsWithDescriptionArgs;
            for (var uid in reports) {
                var report = reports[uid];
                textModel.append({
                                     "text": uid + " " + report.name,
                                     "uid": uid,
                                 });
            }
        }
    }
}
