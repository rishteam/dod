cmake_minimum_required(VERSION 3.10)

project(Editor)

add_executable(Editor
    # main
    EditorApp.cpp
    EditorLayer.cpp

    # Panels
    Panels/SceneHierarchyPanel.cpp
    Panels/ComponentEditPanel.cpp
    Panels/ComponentSelectionPanel.cpp
    Panels/ErrorModal.cpp
    Panels/HelpPanel.cpp
    Panels/AboutPanel.cpp
    Panels/StatusBarPanel.cpp
    Panels/SettingPanel.cpp
    Panels/Animation/AnimationEditor.cpp

    # Edit
    Edit/EditorGrid.cpp
    Edit/EditorGrid.h
    Edit/EditController.cpp
    Edit/Gizmo.cpp
)

target_include_directories(Editor PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(Editor
    Rish::Engine project_options project_warnings
)
