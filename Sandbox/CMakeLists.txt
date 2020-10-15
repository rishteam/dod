cmake_minimum_required(VERSION 3.10)

project(Sandbox)

add_executable(${PROJECT_NAME}
    SandboxApp.cpp
    SandboxLayer.cpp
        PhysicsLayer.cpp PhysicsLayer.h ParticleTestLayer.cpp ParticleTestLayer.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME}
    Rish::Engine project_options project_warnings
)

file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${PROJECT_BINARY_DIR})
