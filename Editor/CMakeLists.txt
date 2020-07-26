cmake_minimum_required(VERSION 3.10)

project(Editor)

add_executable(Editor
    EditorLayer.cpp
    Editor.cpp
)
target_link_libraries(Editor
    Rish::Engine project_options project_warnings
)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config/imgui.ini DESTINATION ${PROJECT_BINARY_DIR})