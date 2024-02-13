pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls.Universal

import BinaryValentineNative

ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: false
    title: qsTr("About %1").arg(VersionInfo.fullToolName)
    modality: Qt.ApplicationModal
    flags: Qt.Dialog
    color: Colors.background

    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width

    Column {
        anchors.centerIn: parent
        spacing: 10

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: "res/logo-big.png"
        }

        AppLabel {
            width: root.width - 50
            textFormat: Label.RichText
            wrapMode: Label.WordWrap
            onLinkActivated: (link) => {
                Qt.openUrlExternally(link);
            }

            text: qsTr("<center><h2>%5 by %6</h2></center>"
                       + "<p>An open-source and free to use executable files static analyzer. "
                       + "Detects a range of various issues, which are usually out of reach "
                       + "of other tools (such as static code analysis).</p>"
                       + "<p><a href='%1'>Official %2 website</a></p>"
                       + "<p><a href='%3'>%2 Github page</a></p>"
                       + "<p>This program uses <a href='https://www.qt.io/'>Qt version %4</a>.</p>").arg(
                      VersionInfo.websiteUrl).arg(VersionInfo.toolName).arg(
                      VersionInfo.githubUrl).arg(VersionInfo.qtVersion).arg(
                      VersionInfo.fullToolName).arg(VersionInfo.authorName)

            MouseArea {
                anchors.fill: parent
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                acceptedButtons: Qt.NoButton
            }
        }
    }
}
