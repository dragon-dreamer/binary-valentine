import QtQuick.Dialogs

FileDialog {
    acceptLabel: qsTr("Scan")
    rejectLabel: qsTr("Cancel")
    fileMode: FileDialog.OpenFiles
    nameFilters: [qsTr("Portable Executable files (*.exe *.dll *.ocx *.sys *.efi *.scr)"), qsTr("All files (*.*)")]
    options: FileDialog.ReadOnly
}
