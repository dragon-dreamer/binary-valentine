pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import BinaryValentineNative

Pane {
    id: rootPane
    property int projectIndex: 0
    property string projectName: {
        const path = projectModel.projectPath;
        if (!path.length) {
            return qsTr("New project")
                    + (projectIndex ? (" (" + (projectIndex + 1) + ")") : "");
        }

        const result = /([^/\\]+)\.[^\./\\]+$/.exec(path);
        return result === null ? path : result[1];
    }

    property string projectShortName: projectName
    property list<string> pathsToScan
    property string existingProjectPath: ""
    required property ProjectModel projectModel
    property bool analysisInProgress : projectModel.analysisInProgress
    property bool hasAnyChanges: projectModel.hasChanges

    Connections {
        target: rootPane.projectModel

        function onRowSelected(index, parentIndex): void {
            projectTreeView.expand(projectTreeView.rowAtIndex(parentIndex));
            treeSelectionModel.setCurrentIndex(index, ItemSelectionModel.SelectCurrent);
        }
    }

    SplitView {
        anchors.fill: parent

        TreeView {
            id: projectTreeView
            model: rootPane.projectModel
            activeFocusOnTab: true
            enabled: !rootPane.analysisInProgress
            rowSpacing: 3

            SplitView.minimumWidth: 200
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            clip: true

            selectionMode: TreeView.SingleSelection
            selectionModel : ItemSelectionModel {
                id: treeSelectionModel
                onCurrentChanged: logic.changeCurrentIndex(currentIndex)
                Component.onCompleted: {
                    if (rootPane.pathsToScan && rootPane.pathsToScan.length) {
                        rootPane.projectModel.setPathsToScan(rootPane.pathsToScan);
                    }
                }
            }

            delegate: TreeViewDelegate {
                id: treeDelegate
                indentation: 8
                implicitWidth: projectTreeView.width
                implicitHeight: label.implicitHeight * 1.2

                required property string nodeType
                required property string nodeName

                contentItem: Text {
                    id: label
                    text: logic.getNodeName(treeDelegate.nodeType, treeDelegate.nodeName)
                    elide: Text.ElideMiddle
                    color: projectTreeView.enabled ? Colors.labelText : Colors.labelTextDisabled
                }

                background: Rectangle {
                    color: treeDelegate.current
                        ? Colors.treeItemBgCurrent
                        : (projectTreeView.enabled
                           && hoverHandler.hovered ? Colors.treeItemBgHovered : "transparent")
                }

                HoverHandler {
                    id: hoverHandler
                }
            }

            ScrollBar.vertical : ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }

        Loader {
            id: viewLoader
            SplitView.minimumWidth: 400
        }

        Component.onCompleted: treeSelectionModel.setCurrentIndex(
                                   rootPane.projectModel.index(0, 0), ItemSelectionModel.Current);
    }

    QtObject {
        id: logic
        function changeCurrentIndex(currentIndex): void {
            viewLoader.setSource(getCurrentComponent(currentIndex),
                                 { "node": rootPane.projectModel.getNode(currentIndex) });
        }

        function getCurrentComponent(currentIndex): string {
            if (currentIndex && currentIndex.valid) {
                switch (rootPane.projectModel.getNode(currentIndex).nodeType) {
                case "project_settings": return "ProjectSettingsView.qml";
                case "rule_selector": return "RuleSelectorView.qml";
                case "report_filter": return "ReportFilterView.qml";
                case "targets": return "TargetsView.qml";
                case "scan_target": return "ScanTargetView.qml";
                case "report_outputs": return "ReportOutputsView.qml";
                case "scan_runner": return "ScanRunnerView.qml";
                default: break;
                }
            }
            return "ProjectSettingsView.qml";
        }

        function getNodeName(nodeId: string, name): string {
            switch (nodeId) {
            case "project_settings": return qsTr("Project settings");
            case "rule_selector": {
                const isGlobal = name;
                return isGlobal ? qsTr("Global selector") : qsTr("Rule selector");
            }
            case "report_filter": return qsTr("%1 report filter").arg(name);
            case "targets": return qsTr("Targets to scan");
            case "scan_target": return name;
            case "report_outputs": return qsTr("Report outputs");
            case "scan_runner": return "Scan";
            default: break;
            }
            return nodeId;
        }
    }
}
