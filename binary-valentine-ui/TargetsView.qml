import QtQuick
import QtQuick.Controls.Universal
import QtQuick.Layouts
import BinaryValentineNative

PropertyView {
    id: root
    required property Targets node
    fillHeight: true

    HeaderLabel {
        applyTopMargin: false
        text: qsTr("Targets to scan")
    }
    DescriptionLabel {
        Layout.leftMargin: 0
        text: qsTr("This section contains files and directories to scan. "
                   + "You can use absolute paths or paths relative to the root path specified in the "
                   + "project settings. You can also drag and drop one or more "
                   + "files and/or directories below to add them to the project.")
    }

    RowLayout {
        Layout.topMargin: 15
        Layout.fillWidth: true

        TextField {
            id: targetPath
            Layout.fillWidth: true
            placeholderText: qsTr("File or folder path to add")
            onTextEdited: targetExistsLabel.visible = false

            Keys.onReturnPressed: logic.addEnteredTargetToProject()
            Keys.onEnterPressed: logic.addEnteredTargetToProject()
        }

        Button {
            id: addNewTargetButton
            text: qsTr("Add")
            enabled: targetPath.text.trim().length > 0
            onClicked: logic.addEnteredTargetToProject();
        }
    }

    Label {
        id: targetExistsLabel
        text: qsTr("This target has already been added to the project.")
        color: Colors.invalidValueText
        visible: false
    }


    FileDropArea {
        Layout.topMargin: 15
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: 100

        onFilesDropped: function(paths) {
            logic.addTargetsToProject(paths);
        }

        Rectangle {
            anchors.fill: parent
            color: 'transparent'
            border.color: Colors.frameBorder
            border.width: 2
            Label {
                anchors.centerIn: parent
                text: qsTr("Drag and drop files/folders here")
                color: Colors.descriptionLabelText
            }
        }
    }

    QtObject {
        id: logic
        function addTargetsToProject(paths: list<string>): void {
            return root.node.addTargets(paths);
        }
        function addEnteredTargetToProject(): void {
            if (logic.addTargetsToProject(targetPath.text)) {
                targetExistsLabel.visible = true;
            } else {
                targetPath.text = "";
            }
        }
    }
}
