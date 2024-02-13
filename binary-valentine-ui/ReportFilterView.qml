pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BinaryValentineNative

PropertyView {
    id: root
    required property ReportFilter node

    HeaderLabel {
        applyTopMargin: false
        text: qsTr("%1 report filter").arg(root.node.reportUid)
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("Custom report filters allow to set up the fine-grained report filtering by report output text using regular expressions."
                   + " Only the applicable reports which have at least one argument in their output are included in the list.")
    }

    Label {
        Layout.topMargin: 5
        Layout.fillWidth: true
        wrapMode: Label.WordWrap
        text: qsTr("<b>Rule name: </b><i>%1</i>").arg(
                  root.node.reportInfo.name)
    }
    Label {
        Layout.fillWidth: true
        wrapMode: Label.WordWrap
        text: qsTr("<b>Report description text: </b><i>%1</i>").arg(
                  root.node.reportInfo.description)
    }

    ComboBox {
        id: filterModeCombo
        Layout.topMargin: 5
        Layout.fillWidth: true
        textRole: "text"
        valueRole: "value"
        model: [
            { text: qsTr("Enable the report when the regular expressions match"),
                value: ReportFilter.ReportFilterModeEnableWhenMatches },
            { text: qsTr("Disable the report when the regular expressions match"),
                value: ReportFilter.ReportFilterModeDisableWhenMatches }
        ]
        Component.onCompleted: currentIndex = indexOfValue(root.node.filterMode)
        onActivated: root.node.filterMode = currentValue
    }
    ComboBox {
        id: filterAggregationModeCombo
        Layout.fillWidth: true
        visible: root.node.descriptionArgs.length > 1
        textRole: "text"
        valueRole: "value"
        model: [
            { text: qsTr("Apply the filter when all regular expressions match"),
                value: ReportFilter.ReportFilterAggregationModeAllMatch },
            { text: qsTr("Apply the filter when any of the regular expressions matches"),
                value: ReportFilter.ReportFilterAggregationModeAnyMatches }
        ]
        Component.onCompleted: currentIndex = indexOfValue(root.node.filterAggregationMode)
        onActivated: root.node.filterAggregationMode = currentValue
    }

    DescriptionLabel {
        Layout.leftMargin: 0
        Layout.topMargin: 5
        text: logic.formatFilterDescription(filterModeCombo.currentValue,
                                            filterAggregationModeCombo.currentValue,
                                            root.node.descriptionArgs.length > 1)
    }

    GridLayout {
        columns: 2
        Layout.fillWidth: true
        flow: GridLayout.TopToBottom
        rows: root.node.descriptionArgs.length

        Repeater {
            model: root.node.descriptionArgs
            Label {
                required property string modelData
                text: qsTr("<b>%1</b> argument ECMAScript regex").arg(modelData)
             }
        }
        Repeater {
            model: root.node.descriptionArgs
            TextField {
                property color defaultTextColor
                required property string modelData
                Layout.fillWidth: true
                text: root.node.getRegex(modelData)
                onTextEdited: {
                    if (root.node.setRegex(modelData, text)) {
                        color = defaultTextColor;
                        font.bold = false;
                    } else {
                        color = Colors.invalidValueText;
                        font.bold = true;
                    }
                }
                Component.onCompleted: defaultTextColor = color
            }
        }
    }


    Button {
        Layout.topMargin: 15
        Layout.alignment: Qt.AlignBottom | Qt.AlignRight
        text: qsTr("Delete filter")
        onClicked: root.node.deleteFilter()
    }

    QtObject {
        id: logic
        function formatFilterDescription(filterMode, aggregationMode, isMultiRegex: bool): string {
            return qsTr("%1 the report when %2:")
            .arg(filterMode === ReportFilter.ReportFilterModeEnableWhenMatches
                 ? qsTr("Include") : qsTr("Exclude"))
            .arg(getAggregationModeDescription(aggregationMode, isMultiRegex));
        }
        function getAggregationModeDescription(aggregationMode, isMultiRegex: bool): string {
            if (!isMultiRegex) {
                return qsTr("the specified regular expressions matches");
            }

            return aggregationMode === ReportFilter.ReportFilterAggregationModeAllMatch
                    ? qsTr("all specified regular expressions match")
                    : qsTr("any of the specified regular expressions matches");
        }
    }
}
