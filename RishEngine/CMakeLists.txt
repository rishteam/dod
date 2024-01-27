cmake_minimum_required(VERSION 3.10)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

project(RishEngine)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif()

include(cmake/StandardProjectSettings.cmake)
# Project option
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if (ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(project_options INTERFACE -ftime-trace)
  endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" OFF)
if(ENABLE_DOXYGEN)
    add_subdirectory(docs)
endif()

option(RL_SHARED_LIB "Build shared lib" OFF)
if(RL_SHARED_LIB)
    target_compile_definitions(RishEngine PUBLIC "RL_SHARED_LIB")
endif()

option(RL_USE_DYNAMIC_LIBS "Use shared library" OFF)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# Config generate for RishEngine
set(CONFIG_H_SOURCE_DIR ${CMAKE_SOURCE_DIR}/)
string(LENGTH ${CONFIG_H_SOURCE_DIR} CONFIG_H_SOURCE_DIR_LEN)
configure_file(${PROJECT_SOURCE_DIR}/config/Config.h.in ${PROJECT_SOURCE_DIR}/include/Rish/Config.h @ONLY)

# TODO: support for dynamic linking for the engine

set(RL_SCRIPT_FILES
    ../NativeScript/Rish/Script/SpriteRotate.cpp
    ../NativeScript/Rish/Script/CameraController.cpp
    ../NativeScript/Rish/Script/PlayerController.cpp
    ../NativeScript/Rish/Script/Spawner.cpp
    ../NativeScript/Rish/Script/Cinemachine2D.cpp
    ../NativeScript/Rish/Script/TestScript.cpp
    ../NativeScript/Rish/Script/BoxEventController.cpp
    ../NativeScript/Rish/Script/MonsterController.cpp
    ../NativeScript/Rish/Script/ObjectController.cpp
)

# Add executable
add_library(RishEngine STATIC
    # Application
    src/Core/Application.cpp

    # Windows only
    src/Platform/SFMLWindow.cpp
    src/Platform/SFMLInput.cpp
    src/Platform/SFMLImage.cpp


    # Animation
    src/Animation/Animation2DSystem.cpp
    src/Animation/Component.cpp
    src/Animation/Animation.cpp

    # utilities
    src/Core/ResManager.cpp
    src/Core/ResHolder.cpp
    src/Core/Log.cpp
    src/Core/FileSystem.cpp
    src/Core/VFS.cpp

    src/Debug/DebugWindow.cpp
    src/Debug/ImGuiLogWindow.cpp

    src/ImGui/ImGui.cpp
    src/ImGui/MenuAction.cpp
    src/ImGui/AttributeWidget.cpp
    src/ImGui/ImGuiWindow.cpp

    src/Utils/uuid.cpp
    src/Utils/String.cpp

    # fs
    src/Utils/FileDialog.cpp

    # Layer
    src/Layer/Layer.cpp
    src/Layer/LayerStack.cpp
    src/Layer/ImGuiLayer.cpp

    # Time
    src/Core/Time.cpp

    # Effect
    src/Effect/Particle/ParticleSystem.cpp
    src/Effect/Light/LightSystem.cpp

    # Box
    src/Collider/Box.cpp
    src/Collider/ColliderSystem.cpp

    # Physics
    src/Physics/Arbiter.cpp
    src/Physics/Joint.cpp
    src/Physics/PhysicsWorld.cpp
    src/Physics/RigidBody2D.cpp
    src/Physics/vector_math.cpp
    src/Physics/PhysicsSystem.cpp
    src/Physics/Component.cpp

    # Renderer
    src/Renderer/Shader.cpp
    src/Renderer/VertexArray.cpp
    src/Renderer/Buffer.cpp
    src/Renderer/Texture2D.cpp
    src/Renderer/SubTexture2D.cpp
    src/Renderer/Framebuffer.cpp
    src/Renderer/RendererCommand.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Renderer2D.cpp

    # Camera
    src/Renderer/Camera/OrthographicCamera.cpp
    src/Renderer/Camera/OrthographicCameraController.cpp

    # Scene
    src/Scene/Scene.cpp
    src/Scene/Component.cpp
    src/Scene/Entity.cpp
    src/Scene/ComponentManager.cpp
    src/Scene/SceneCamera.cpp
    src/Scene/ScriptableManager.cpp
    src/Scene/ScriptableEntity.cpp
    src/Scene/System/NativeScriptSystem.cpp
    src/Scene/SystemManager.cpp

    # Math
    src/Math/AABB.cpp
    src/Math/BoundingBox.cpp
    src/Math/Random.cpp

    src/Scene/System/SpriteRenderSystem.cpp

    # Sound
    src/Platform/SFMLListener.cpp
    src/Sound/SoundSystem.cpp
    src/Sound/Sound.cpp

    ${RL_SCRIPT_FILES}
)

# alias
add_library(Rish::Engine ALIAS RishEngine)
target_include_directories(RishEngine
    PUBLIC ${PROJECT_SOURCE_DIR}/include

    # Scripts
    PUBLIC ${CMAKE_SOURCE_DIR}/NativeScript
)
target_link_libraries(RishEngine project_options project_warnings) # project options and warnings
target_precompile_headers(RishEngine PUBLIC ${PROJECT_SOURCE_DIR}/include/Rish/rlpch.h) # precompiled headers

if(RL_USE_DYNAMIC_LIBS)
    target_compile_definitions(RishEngine PUBLIC "RL_EXPORTS")
endif()

if (MSYS AND MSYS_STATIC_RUNTIME)
    target_link_libraries(${PROJECT_NAME}
        -static-libgcc -static-libstdc++ -lwsock32 -lws2_32
        -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
    )
endif()

set(BUILD_SHARED_LIBS ${RL_USE_DYNAMIC_LIBS})

# Add SFML ###############################
if(APPLE)
    set(SFML_STATIC_LIBRARIES False)
    set(SFML_DIR "/usr/local/Cellar/sfml/2.5.1")
    target_compile_definitions(RishEngine PUBLIC "GL_SILENCE_DEPRECATION")

    # Add Boost::filesystem
    set(BOOST_ROOT "/usr/local/Cellar/boost/1.72.0_2")
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    target_include_directories(RishEngine PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(RishEngine ${Boost_LIBRARIES})
elseif(MSYS)
    set(SFML_STATIC_LIBRARIES False)
    #    set(SFML_DIR "/mingw32/lib/cmake/SFML")
    set(SFML_DIR ${CMAKE_SOURCE_DIR}/deps/SFML)
else()
    set(SFML_STATIC_LIBRARIES False)
    #    set(SFML_DIR "/mingw32/lib/cmake/SFML")
    set(SFML_DIR ${CMAKE_SOURCE_DIR}/deps/SFML)
endif()
add_subdirectory(deps/SFML)
if (WIN32 AND SFML_BUILD_AUDIO)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/deps/SFML/extlibs/bin/x64/openal32.dll $<TARGET_FILE_DIR:RishEngine>
    )
endif()
#find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(RishEngine
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)

# Add imgui ###############################
#add_subdirectory(deps/imgui)
#target_link_libraries(RishEngine
#    imgui::imgui
#)

# Add ImGui-SFML ###############################
#find_package(ImGui-SFML REQUIRED)
#if(NOT ImGui-SFML_FOUND)
#    message(FATAL_ERROR "ImGui-SFML not found")
#endif()
add_subdirectory(deps/imgui-sfml)
target_link_libraries(RishEngine
    ImGui-SFML::ImGui-SFML
)

# Add OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif()
target_include_directories(RishEngine
    PUBLIC ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(RishEngine
    ${OPENGL_LIBRARIES}
)

# Add nlohmann_json
add_subdirectory(deps/nlohmann_json)
#find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(RishEngine
    nlohmann_json::nlohmann_json
)

# Add spdlog
set(SPDLOG_BUILD_SHARED OFF)
add_subdirectory(deps/spdlog)
target_link_libraries(RishEngine
    spdlog::spdlog
)

# Add fmt
add_subdirectory(deps/fmt)
target_link_libraries(RishEngine
    fmt::fmt
)

# Add glm
add_subdirectory(deps/glm)
target_link_libraries(RishEngine
    glm::glm
)

# Add nativefiledialog
add_subdirectory(deps/nativefiledialog)
target_link_libraries(RishEngine
    nfd::nfd
)

# Add glad
add_subdirectory(deps/glad)
target_link_libraries(RishEngine
    glad::glad
)

# Add EnTT
add_subdirectory(deps/entt)
target_link_libraries(RishEngine
    EnTT::EnTT
)

# Add cereal
#find_package(cereal REQUIRED)
set(BUILD_DOC OFF)
set(BUILD_SANDBOX OFF)
set(SKIP_PERFORMANCE_COMPARISON ON)
add_subdirectory(deps/cereal)
target_link_libraries(RishEngine
    cereal
)

add_subdirectory(deps/IconFontCppHeaders)
target_link_libraries(RishEngine IconFontCpp::IconFontCpp)

include(FetchContent)

# Add re2
#find_package(re2 REQUIRED)
set(ABSL_ENABLE_INSTALL ON)
set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_SHARED_LIBS OFF)
add_compile_options(-DABSL_FORCE_WAITER_MODE=4)
FetchContent_Declare(
    abseil-cpp
    GIT_REPOSITORY  https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20230802.1
)
FetchContent_MakeAvailable(abseil-cpp)
#target_link_libraries(RishEngine
#    absl::base
#    absl::core_headers
#    absl::fixed_array
#    absl::flags
#    absl::flat_hash_map
#    absl::flat_hash_set
#    absl::inlined_vector
#    absl::optional
#    absl::span
#    absl::str_format
#    absl::strings
#    absl::synchronization
#)

set(BUILD_SHARED_LIBS OFF)
FetchContent_Declare(
    re2
    GIT_REPOSITORY https://github.com/google/re2.git
    GIT_TAG 2023-11-01
)
FetchContent_MakeAvailable(re2)
target_link_libraries(RishEngine
    re2::re2
)

# Copy files
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
# file(GLOB BINARY_DEP_DLLS
#     "${CMAKE_SOURCE_DIR}/bin/*.dll"
# )
# file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
