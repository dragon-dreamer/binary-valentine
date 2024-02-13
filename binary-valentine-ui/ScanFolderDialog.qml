import QtQuick.Dialogs

FolderDialog {
    acceptLabel: qsTr("Scan")
    rejectLabel: qsTr("Cancel")
    options: FolderDialog.ReadOnly
}
