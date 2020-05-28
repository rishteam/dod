cmake_minimum_required(VERSION 3.10)

project(RishEngine)

set(CMAKE_BUILD_TYPE Debug)

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
option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" ON)
if(ENABLE_DOXYGEN)
    add_subdirectory(docs)
endif()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# Dependencies
add_subdirectory(deps/nativefiledialog)

# TODO: move this to sandbox or leave only testmain.cpp for testing purpose only
# option(DOD_USE_TEST_MAIN "Use test main" NO)
# if(DOD_USE_TEST_MAIN)
#     set(DOD_MAIN_FILE src/testmain.cpp)
# else()
#     set(DOD_MAIN_FILE src/main.cpp)
# endif()

# Config generate for RishEngine
set(CONFIG_H_SOURCE_DIR ${PROJECT_SOURCE_DIR}/)
string(LENGTH ${CONFIG_H_SOURCE_DIR} CONFIG_H_SOURCE_DIR_LEN)
configure_file(${PROJECT_SOURCE_DIR}/config/Config.h.in ${PROJECT_SOURCE_DIR}/include/Rish/Config.h @ONLY)

# TODO: support for dynamic linking for the engine

# Add executable
add_library(RishEngine
    # Application
    src/Core/Application.cpp

    # Window
    src/Platform/SFMLWindow.cpp

    # Animation
    src/Animation/Animation.cpp
    src/Animation/Loader.cpp
    # Animation Editor
    src/Animation/Editor/Editor.cpp

    # utilities
    src/Core/ResManager.cpp
    src/Core/ResHolder.cpp
    src/Core/Log.cpp

    # fs
    src/Utils/FileDialog.cpp

    # Layer
    src/Layer/Layer.cpp
    src/Layer/LayerStack.cpp
)
# alias
add_library(Rish::Engine ALIAS RishEngine)
target_include_directories(RishEngine
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(RishEngine project_options project_warnings) # project options and warnings
target_precompile_headers(RishEngine PUBLIC ${PROJECT_SOURCE_DIR}/include/Rish/rlpch.h) # precompiled headers

option(RL_USE_STATIC "Use RishEngine as a static libaray" ON)
if(RL_USE_STATIC)
    target_compile_definitions(RishEngine PUBLIC "RL_USE_STATIC")
endif()

option(RL_BUILD_DLL "Build dll" OFF)
if(RL_BUILD_DLL)
    target_compile_definitions(RishEngine PUBLIC "RL_BUILD_DLL")
endif()

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
    set(SFML_STATIC_LIBRARIES True)
    set(SFML_DIR "/mingw32/lib/cmake/SFML")
else()
    set(SFML_STATIC_LIBRARIES False)
    set(SFML_DIR "/mingw32/lib/cmake/SFML")
#    message(FATAL_ERROR "Not supported")
endif()

# Add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(RishEngine
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)

# Add ImGui-SFML
find_package(ImGui-SFML REQUIRED)
if(NOT ImGui-SFML_FOUND)
    message(FATAL_ERROR "ImGui-SFML not found")
endif()
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

# Add fmt
find_package(fmt REQUIRED)
if(NOT fmt_FOUND)
    message(FATAL_ERROR "fmt not found")
endif()
target_link_libraries(RishEngine
    fmt::fmt
)

# Add nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(RishEngine
    nlohmann_json::nlohmann_json
)

# Add spdlog
find_package(spdlog REQUIRED)
target_link_libraries(RishEngine
    spdlog::spdlog
)

# Add nativefiledialog
target_link_libraries(RishEngine
    nfd::nfd
)

if(MSYS)
    target_link_libraries(RishEngine
        # -lwsock32 -lws2_32 -static-libgcc -static-libstdc++
        # -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
        # -mconsole / -mwindows
    )
endif()

# Copy files
# file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
# file(GLOB BINARY_DEP_DLLS
#     "${CMAKE_SOURCE_DIR}/bin/*.dll"
# )
# file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
