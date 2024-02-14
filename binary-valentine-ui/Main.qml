pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Dialogs as Dlg
import QtQuick.Layouts
import QtQuick.Controls.Universal

import "UrlHelper.js" as UrlHelper

import BinaryValentineNative

ApplicationWindow {
    width: 1024
    height: 768
    minimumWidth: 640
    minimumHeight: 480
    visible: true
    title: VersionInfo.fullToolName
    color: Colors.background

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")

            Action {
                text: qsTr("&New project")
                enabled: !logic.modalDialogInProgress
                shortcut: StandardKey.New
                onTriggered: {
                    tabs.addNewTab();
                    logic.addNewHomeView();
                }
            }
            MenuSeparator { }

            Action {
                text: qsTr("&Save project")
                enabled: logic.isProjectTab && !logic.modalDialogInProgress
                shortcut: StandardKey.Save
                onTriggered: {
                    logic.saveCurrentProject();
                }
            }
            Action {
                text: qsTr("Save project &as...")
                enabled: logic.isProjectTab && !logic.modalDialogInProgress
                shortcut: StandardKey.SaveAs
                onTriggered: {
                    logic.saveCurrentProjectAs();
                }
            }
            MenuSeparator { }

            Action {
                text: qsTr("Close current tab")
                enabled: !logic.modalDialogInProgress
                shortcut: StandardKey.Close
                onTriggered: {
                    logic.closeCurrentProject();
                }
            }

            Action {
                text: qsTr("&Quit")
                enabled: !logic.modalDialogInProgress
                shortcut: StandardKey.Quit
                onTriggered: logic.quitApp()
            }
        }

        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                onTriggered: {
                    aboutWindow.visible = true;
                }
            }
            Action {
                text: qsTr("&Online docs")
                onTriggered: {
                    Qt.openUrlExternally(VersionInfo.docsWebsiteUrl);
                }
            }
            Action {
                text: qsTr("&Github page")
                onTriggered: {
                    Qt.openUrlExternally(VersionInfo.githubUrl);
                }
            }
        }
    }

    ProjectTabBar {
        id: tabs
        width: parent.width

        onTabCloseRequested: (index) => {
            logic.tabCloseRequested(index);
        }
    }

    StackLayout {
        id: tabStackLayout
        width: parent.width
        anchors.top: tabs.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 2
        currentIndex: tabs.currentIndex

        Repeater {
            model: ObjectModel {
                id: tabContent
            }
        }

        onCurrentIndexChanged: logic.updateIsProjectTab()
    }

    Dialog {
        id: projectErrorDialog
        property alias projectErrorText: projectErrorTextLabel.text

        modal: true
        visible: false
        standardButtons: Dialog.Ok
        anchors.centerIn: parent
        width: 400

        Label {
            id: projectErrorTextLabel
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }
    }

    Dialog {
        id: analysisAbortDialog

        title: qsTr("Project analysis in progress")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400

        Label {
            text: qsTr("The project analysis is currently in progress. "
                       + "Would you like to abort it and close the project?")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }

        onAccepted: logic.abortProjectAnalysis()
    }

    Dialog {
        id: unsavedChangesDialog

        title: qsTr("Project has unsaved changes")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400

        Label {
            text: qsTr("The project has unsaved changes. "
                       + "Are you sure you want to close it without saving?")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }

        onAccepted: logic.closeProject(true)
    }

    Dialog {
        id: analysisAbortWaitDialog

        title: qsTr("Aborting analysis...")
        standardButtons: Dialog.Cancel
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400

        Label {
            text: qsTr("Please wait while project analysis is being aborted...")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }

        onClosed: logic.stopWaitingForAnalysisAbort()
    }

    Dialog {
        id: closeAppUnsavedChangesDialog

        title: qsTr("Projects with unsaved changes")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400

        Label {
            text: qsTr("You have one or more projects with unsaved changes. "
                       + "Would you like to ignore them and close the application?")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }

        onAccepted: logic.quitAppWithUnsavedChanges();
    }

    Dialog {
        id: closeAppAnalysisInProgressDialog

        title: qsTr("Analysis in progress")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400

        Label {
            text: qsTr("You have one or more projects which are currently being analyzed. "
                       + "Would you like to abort analysis and close the application?")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }

        onAccepted: logic.quitAppWithAbortedAnalysis();
    }

    Dialog {
        id: closeAppAbortAnalysisDialog

        title: qsTr("Aborting analysis...")
        modal: true
        visible: false
        anchors.centerIn: parent
        width: 400
        closePolicy: Dialog.NoAutoClose

        Label {
            text: qsTr("Please wait until project analysis is stopped...")
            width: parent.width
            height: parent.height
            wrapMode: Label.WordWrap
        }
    }

    AboutWindow {
        id: aboutWindow
        visible: false
    }

    ProjectModelLoader {
        id: modelLoader
    }

    Dlg.FileDialog {
        id: saveProjectDialog
        acceptLabel: qsTr("Save")
        rejectLabel: qsTr("Cancel")
        fileMode: Dlg.FileDialog.SaveFile
        nameFilters: [qsTr("Project files (*.xml)"), qsTr("All files (*.*)")]
        onAccepted: logic.saveProjectPathSelected(selectedFile)
    }

    onClosing: (close) => {
        logic.closeApp(close);
    }

    QtObject {
        id: logic
        property int projectIndex: 0
        property int projectBeingClosed: 0
        property bool isProjectTab: false
        property bool shouldCloseAppWithUnsavedChanges: false
        property bool modalDialogInProgress: false
        property ProjectModel modelWaitingForAnalysisAbort

        function addNewHomeView(): void {
            const homeView = Qt.createComponent("HomeView.qml");
            const obj = homeView.createObject(tabStackLayout);
            tabContent.append(obj);
            obj.newProjectRequested.connect(() => createNewProject(obj.StackLayout.index, null, null));
            obj.fileScanRequested.connect((pathsToScan)
                                          => createNewProject(obj.StackLayout.index, pathsToScan, null));
            obj.existingProjectOpened.connect((path)
                                              => createNewProject(obj.StackLayout.index, null, path))
        }

        function updateIsProjectTab(): void {
            isProjectTab = (tabStackLayout.children[tabs.currentIndex] as ProjectView) !== null
        }

        function createNewProject(viewIndex: int, pathsToScan, existingProjectPath): void {
            const model = existingProjectPath === null
                    ? modelLoader.createNewProjectModel()
                    : modelLoader.createNewProjectModelFromProject(existingProjectPath);

            if (model === null) {
                projectErrorDialog.title = qsTr("Unable to load the project");
                projectErrorDialog.projectErrorText = modelLoader.getLastError();
                openDialog(projectErrorDialog);
                return;
            }

            const component = Qt.createComponent("ProjectView.qml");
            var properties = { "projectModel": model, "projectIndex": logic.projectIndex++ };
            if (pathsToScan) {
                properties["pathsToScan"] = pathsToScan;
            }

            const view = component.createObject(tabStackLayout, properties);
            tabs.renameTab(viewIndex,
                           Qt.binding(() => view.projectShortName),
                           Qt.binding(() => view.hasAnyChanges));
            tabContent.insert(viewIndex + 1, view);
            tabContent.remove(viewIndex);

            updateIsProjectTab();
        }

        function saveCurrentProject(path): void {
            if (!isProjectTab) {
                return;
            }

            const currentItemModel = (tabStackLayout.children[tabs.currentIndex] as ProjectView).projectModel;

            var errorText = "";
            if (path === undefined && currentItemModel.projectPath.length) {
                errorText = currentItemModel.saveToFile(currentItemModel.projectPath);
            } else {
                if (path === undefined) {
                    saveProjectDialog.open();
                } else {
                    errorText = currentItemModel.saveToFile(path);
                }
            }

            if (errorText.length) {
                projectErrorDialog.title = qsTr("Unable to save the project");
                projectErrorDialog.projectErrorText = errorText;
                openDialog(projectErrorDialog);
            }
        }

        function saveCurrentProjectAs(): void {
            //TODO: set dialog path
            saveProjectDialog.open();
        }

        function saveProjectPathSelected(path: url): void {
            const helper = new UrlHelper.UrlHelper(path);
            if (helper.areLocalFiles()) {
                var localPaths = helper.getLocalFilePaths();
                if (localPaths.length === 1) {
                    saveCurrentProject(localPaths[0]);
                }
            }
        }

        function closeCurrentProject(confirmUnsavedChanges): void {
            projectBeingClosed = tabs.currentIndex;
            closeProject(confirmUnsavedChanges);
        }

        function selectNearestTab(index: int): void {
            if (tabs.currentIndex === index && tabContent.count > index + 1) {
                tabs.selectTab(index + 1);
            }
        }

        function closeProject(confirmUnsavedChanges): void {
            stopWaitingForAnalysisAbort();

            analysisAbortWaitDialog.close();

            const view = tabStackLayout.children[projectBeingClosed] as ProjectView;
            if (!view) {
                if (tabContent.count > 1) {
                    selectNearestTab(projectBeingClosed);
                    tabContent.remove(projectBeingClosed);
                    tabs.closeTab(projectBeingClosed);
                    updateIsProjectTab();
                }

                return;
            }

            const currentModel = view.projectModel;
            if (currentModel.analysisInProgress) {
                openDialog(analysisAbortDialog);
                return;
            }

            if (currentModel.hasChanges && !confirmUnsavedChanges) {
                openDialog(unsavedChangesDialog);
                return;
            }

            if (tabContent.count === 1) {
                tabs.addNewTab();
                addNewHomeView();
            }

            selectNearestTab(projectBeingClosed);
            tabContent.remove(projectBeingClosed);
            tabs.closeTab(projectBeingClosed);
            updateIsProjectTab();
        }

        function stopWaitingForAnalysisAbort(): void {
            if (modelWaitingForAnalysisAbort) {
                modelWaitingForAnalysisAbort.analysisInProgressChanged.disconnect(closeProject);
                modelWaitingForAnalysisAbort = null;
            }
        }

        function abortProjectAnalysis(): void {
            const view = tabStackLayout.children[projectBeingClosed] as ProjectView;
            if (!view) {
                return;
            }

            const currentModel = view.projectModel;
            if (!currentModel.analysisInProgress) {
                closeProject();
                return;
            }

            openDialog(analysisAbortWaitDialog);
            modelWaitingForAnalysisAbort = currentModel;
            currentModel.analysisInProgressChanged.connect(closeProject);
            currentModel.requestAnalysisStop();
        }

        function quitApp(): void {
            Qt.quit();
        }

        function closeApp(closeConfirmation): void {
            if (analysisAbortWaitDialog.visible
                    || unsavedChangesDialog.visible
                    || analysisAbortDialog.visible
                    || closeAppAbortAnalysisDialog.visible
                    || closeAppAnalysisInProgressDialog.visible
                    || closeAppUnsavedChangesDialog.visible) {
                closeConfirmation.accepted = false;
                return;
            }

            var hasAnalysisInProgress = false;
            var hasUnsavedChanges = false;

            const children = tabContent.children;
            for (var i = 0, c = tabContent.count; i !== c; ++i) {
                const view = tabContent.get(i) as ProjectView;
                if (view) {
                    const currentModel = view.projectModel;
                    if (currentModel.analysisInProgress) {
                        hasAnalysisInProgress = true;
                    }
                    if (currentModel.hasChanges) {
                        hasUnsavedChanges = true;
                    }
                }
            }

            if (hasAnalysisInProgress) {
                closeConfirmation.accepted = false;
                openDialog(closeAppAnalysisInProgressDialog);
                return;
            }

            if (hasUnsavedChanges && !shouldCloseAppWithUnsavedChanges) {
                closeConfirmation.accepted = false;
                openDialog(closeAppUnsavedChangesDialog);
                return;
            }

            shouldCloseAppWithUnsavedChanges = false;
        }

        function quitAppWithUnsavedChanges(): void {
            shouldCloseAppWithUnsavedChanges = true;
            Qt.quit();
        }

        function quitAppWithAbortedAnalysis(): void {
            var analyzedModels = [];
            const children = tabContent.children;
            for (var i = 0, c = tabContent.count; i !== c; ++i) {
                const view = tabContent.get(i) as ProjectView;
                if (view) {
                    const currentModel = view.projectModel;
                    if (currentModel.analysisInProgress) {
                        analyzedModels.push(currentModel);
                    }
                }
            }

            if (!analyzedModels.length) {
                quitApp();
                return;
            }

            var countObject = {
                count: analyzedModels.length
            };

            for (i = 0, c = analyzedModels.length; i !== c; ++i) {
                addAbortAnalysisSlotConnection(analyzedModels[i], countObject);
                analyzedModels[i].requestAnalysisStop();
            }

            openDialog(closeAppAbortAnalysisDialog);
        }

        function addAbortAnalysisSlotConnection(model: ProjectModel, countObject): void {
            var slotConnection = () => {
                model.analysisInProgressChanged.disconnect(slotConnection);

                if (--countObject.count === 0) {
                    closeAppAbortAnalysisDialog.close();
                    quitApp();
                }
            };

            model.analysisInProgressChanged.connect(slotConnection);
        }

        function openDialog(dialogId): void {
            var slotConnection = () => {
                modalDialogInProgress = false;
                dialogId.onClosed.disconnect(slotConnection);
            };
            dialogId.onClosed.connect(slotConnection);
            modalDialogInProgress = true;
            dialogId.open();
        }

        function tabCloseRequested(index: int): void {
            if (tabContent.count === 1 && !isProjectTab) {
                return;
            }

            logic.projectBeingClosed = index;
            closeProject();
        }
    }

    Component.onCompleted: {
        logic.addNewHomeView();
    }
}
