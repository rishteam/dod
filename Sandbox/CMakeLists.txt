cmake_minimum_required(VERSION 3.10)

project(Sandbox)

add_executable(${PROJECT_NAME}
    SandboxApp.cpp
    SandboxLayer.cpp 
#    PhysicsLayer.cpp
#    ParticleTestLayer.cpp
#    ScriptTestLayer.cpp
#    LightLayer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    Rish::Engine project_options project_warnings
)
