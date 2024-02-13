import QtQuick.Dialogs

FileDialog {
    acceptLabel: qsTr("Open")
    rejectLabel: qsTr("Cancel")
    fileMode: FileDialog.OpenFile
    nameFilters: [qsTr("Project files (*.xml)"), qsTr("All files (*.*)")]
    options: FileDialog.ReadOnly
}

