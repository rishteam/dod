cmake_minimum_required(VERSION 3.10)

project(Sandbox)

add_executable(Sandbox
    SandboxApp.cpp
)
target_link_libraries(Sandbox
    Rish::Engine project_options project_warnings
)

