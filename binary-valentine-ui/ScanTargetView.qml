pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import BinaryValentineNative

PropertyView {
    id: root
    required property ScanTarget node

    HeaderLabel {
        applyTopMargin: false
        text: root.node.path
        elide: Text.ElideMiddle
        Layout.fillWidth: true
    }

    RowLayout {
        TextField {
            id: newPathTextField
            text: root.node.path
            Layout.fillWidth: true
            onTextEdited: unableToRenameLabel.visible = false
        }
        Button {
            text: qsTr("Change path")
            onClicked: {
                var text = newPathTextField.text.trim();
                if (root.node.path === text) {
                    unableToRenameLabel.visible = false;
                    return;
                }

                if (text.length) {
                    unableToRenameLabel.visible =
                            !root.node.setPath(newPathTextField.text);
                }
            }
        }
    }
    Label {
        id: unableToRenameLabel
        text: qsTr("Unable to change the target path. Another target with the same path already exist in the project.")
        color: Colors.invalidValueText
        visible: false
    }

    Switch {
        Layout.topMargin: 15
        id: recursiveScan
        checked: root.node.isRecursive
        leftPadding: 0
        text: qsTr("Scan recursively")
        onToggled: root.node.isRecursive = checked
    }
    DescriptionLabel {
        target: recursiveScan
        text: recursiveScan.checked
              ? qsTr("If the target is a directory, scan it recursively.")
              : qsTr("If the target is a directory, do not scan it recursively.")
    }

    Button {
        Layout.topMargin: 10
        text: qsTr("Set up target-specific rule selector")
        onClicked: root.node.addRuleSelector()
    }

    HeaderLabel {
        text: qsTr("Path options")
    }
    RowLayout {
        Button {
            text: qsTr("Use relative path")
            onClicked: root.node.makeRelative()
        }
        Button {
            text: qsTr("Use absolute path")
            onClicked: root.node.makeAbsolute()
        }
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("You can make this target path relative if it is under the root path specified in the project settings.")
    }

    HeaderLabel {
        text: qsTr("Path filters")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("This path will be scanned only if it matches all of the ECMAScript regular expressions below. "
                   + "You can use these regular expressions to filter out nested subdirectories and files which should not be scanned.")
    }

    ListModel {
        id: listModel
        Component.onCompleted: logic.loadPathFilters(listModel)
    }
    Link {
        text: qsTr("Add new")
        onLinkActivated: logic.addNewPathFilter(listModel)
    }

    Frame {
        Layout.fillWidth: true
        background: Rectangle {
            color: "transparent"
            border.color: Colors.frameBorder
            radius: 2
        }
        ScrollView  {
            id: reportsScrollView
            width: parent.width
            contentHeight: pathFilterList.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            ListView {
                id: pathFilterList
                model: listModel
                height: 150
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                clip: true
                pixelAligned: true
                highlightMoveDuration: 0
                highlightResizeDuration: 0
                spacing: 5

                delegate: RowLayout {
                    width: ListView.view.width - 20
                    required property var model
                    required property int index

                    ComboBox {
                        model: [
                            { value: ScanTarget.IncludePathWhenMatches,
                                text: qsTr("Include when path matches") },
                            { value: ScanTarget.ExcludePathWhenMatches,
                                text: qsTr("Exclude when path matches") },
                        ]
                        implicitContentWidthPolicy: ComboBox.WidestText
                        textRole: "text"
                        valueRole: "value"
                        Component.onCompleted: currentIndex = indexOfValue(parent.model.pathFilterType)
                        onActivated: logic.setPathFilterType(parent.index, currentValue)
                    }

                    TextField {
                        property color defaultColor

                        Layout.fillWidth: true
                        text: parent.model.regex
                        Component.onCompleted: defaultColor = color
                        onTextEdited: {
                            if (!logic.setPathFilterRegex(parent.index, text)) {
                                color = Colors.invalidValueText;
                                font.bold = true;
                            } else {
                                color = defaultColor;
                                font.bold = false;
                            }
                        }
                    }
                    Button {
                        text: qsTr("Delete")
                        onClicked: logic.deletePathFilter(listModel, parent.index)
                    }
                }
            }
        }
    }

    Button {
        Layout.topMargin: 15
        Layout.alignment: Qt.AlignBottom | Qt.AlignRight
        text: qsTr("Delete target")
        onClicked: root.node.deleteTarget()
    }

    QtObject {
        id: logic

        function addNewPathFilter(model): void {
            model.append(root.node.addNewPathFilter());
        }

        function deletePathFilter(model, index: int): void {
            model.remove(index);
            root.node.deletePathFilter(index);
        }

        function loadPathFilters(model): void {
            const filters = root.node.getPathFilters();
            for (var key in filters) {
                model.append(filters[key]);
            }
        }

        function setPathFilterType(index: int, value): void {
            root.node.setPathFilterType(index, value);
        }

        function setPathFilterRegex(index: int, text: string): bool {
            return root.node.setPathFilterRegex(index, text);
        }
    }
}
