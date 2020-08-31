cmake_minimum_required(VERSION 3.10)

project(Editor)

add_executable(Editor
    # main
    EditorLayer.cpp
    Editor.cpp

    # Panels
    Panels/SceneHierarchyPanel.cpp
    Panels/ComponentEditPanel.cpp)

target_include_directories(Editor PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(Editor
    Rish::Engine project_options project_warnings
)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})
if(EXISTS ${PROJECT_SOURCE_DIR}/config/imgui.ini)
    file(COPY ${PROJECT_SOURCE_DIR}/config/imgui.ini DESTINATION ${PROJECT_BINARY_DIR})
endif()
