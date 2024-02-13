cmake_minimum_required(VERSION 3.16)

project(binary-valentine-ui VERSION 0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup()

if (WIN32)
    set(AppIconResourcesWindows "${CMAKE_CURRENT_SOURCE_DIR}/win32/binary_valentine_ui.rc")
endif()

qt_add_executable(appbinary-valentine-ui
    main.cpp
    "${AppIconResourcesWindows}"
)

set_source_files_properties(Colors.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appbinary-valentine-ui
    URI binary-valentine-ui
    VERSION 1.0
    QML_FILES Main.qml
    QML_FILES HomeView.qml
    QML_FILES Link.qml
    QML_FILES UrlHelper.js
    QML_FILES ScanFileDialog.qml
    QML_FILES ScanFolderDialog.qml
    QML_FILES DynamicTabButton.qml
    QML_FILES ProjectTabBar.qml
    QML_FILES ProjectTabButton.qml
    QML_FILES OpenProjectDialog.qml
    QML_FILES FileDropArea.qml
    QML_FILES ProjectView.qml
    QML_FILES ProjectSettingsView.qml
    QML_FILES RuleSelectorView.qml
    QML_FILES DescriptionLabel.qml
    QML_FILES HeaderLabel.qml
    QML_FILES PropertyView.qml
    QML_FILES ReportFilterView.qml
    QML_FILES TargetsView.qml
    QML_FILES ScanTargetView.qml
    QML_FILES ReportOutputsView.qml
    QML_FILES ScanRunnerView.qml
    QML_FILES CheckableButton.qml
    RESOURCES res/level-0.png res/level-1.png res/level-2.png res/level-3.png res/level-no.png res/level-progress.png res/logo.png res/logo-big.png res/logo-small.png
    QML_FILES ProjectViewLoader.qml
    QML_FILES AboutWindow.qml
    RESOURCES ../data/all_winapi.xml ../data/apisets.xml ../data/not_recommended_winapi.xml
    QML_FILES Colors.qml
    QML_FILES AppLabel.qml
)

qt_add_qml_module(BinaryValentineNative
    URI BinaryValentineNative
    VERSION 1.0
    SOURCES ProjectTreeNode.h
    SOURCES ProjectTreeNode.cpp
    SOURCES ProjectRootNode.h ProjectRootNode.cpp
    SOURCES ProjectModel.h ProjectModel.cpp
    SOURCES VersionInfo.h VersionInfo.cpp
    SOURCES RuleSelectorNode.h RuleSelectorNode.cpp
    SOURCES ProjectSettingsNode.h ProjectSettingsNode.cpp
    SOURCES ImmutableContext.h ImmutableContext.cpp
    SOURCES BinaryValentineContext.h BinaryValentineContext.cpp
    SOURCES ReportFilterNode.h ReportFilterNode.cpp
    SOURCES TargetsNode.h TargetsNode.cpp
    SOURCES ScanTargetNode.h ScanTargetNode.cpp
    SOURCES ReportOutputsNode.h ReportOutputsNode.cpp
    SOURCES ScanRunnerNode.h ScanRunnerNode.cpp
    SOURCES ModelToAnalysisPlanConverter.h ModelToAnalysisPlanConverter.cpp
    SOURCES QtStdTypeConverter.h QtStdTypeConverter.cpp
    SOURCES ProgressReport.h ProgressReport.cpp
    SOURCES AnalysisRunner.h AnalysisRunner.cpp
    SOURCES ProjectTreeNodes.h
    SOURCES AnalysisProgress.h AnalysisProgress.cpp
    SOURCES InMemoryReportOutput.h InMemoryReportOutput.cpp
    SOURCES AnalyzedFileTracker.h AnalyzedFileTracker.cpp
    SOURCES AnalysisPlanModelPopulator.h AnalysisPlanModelPopulator.cpp
    SOURCES ProjectModelLoader.h ProjectModelLoader.cpp
    SOURCES AnalysisPlanXmlSaver.h AnalysisPlanXmlSaver.cpp
    SOURCES ExceptionFormatter.h ExceptionFormatter.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appbinary-valentine-ui PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appbinary-valentine-ui
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appbinary-valentine-ui
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::QuickDialogs2 BinaryValentineNative
)

target_link_libraries(BinaryValentineNative PRIVATE binary_valentine_lib utilities)
target_link_libraries(BinaryValentineNative PRIVATE "$<$<CONFIG:DEBUG>:Qt6::Test>")

target_compile_features(appbinary-valentine-ui PRIVATE cxx_std_20)
target_compile_features(BinaryValentineNative PRIVATE cxx_std_20)

include(GNUInstallDirs)
install(TARGETS appbinary-valentine-ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
