cmake_minimum_required(VERSION 3.10)

project(dod)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug) # Debug Release

add_subdirectory(deps/nativefiledialog)

# Add executable
add_executable(dod
    src/main.cpp

    # Animation
    # src/animation/animation.cpp
    # src/animation/loader.cpp

    # Animation Editor
    # src/animation/editor/editor.cpp

    # utilities
    src/core/resManager.cpp
    src/core/resHolder.cpp
    src/core/log.cpp
    # fs
    src/utils/filedialog.cpp
)
target_include_directories(dod
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(dod
    PUBLIC -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Woverloaded-virtual -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wno-sign-compare
)

if(APPLE)
    set(SFML_STATIC_LIBRARIES False)
    set(SFML_DIR "/usr/local/Cellar/sfml/2.5.1")
    target_compile_definitions(dod PUBLIC "GL_SILENCE_DEPRECATION")
elseif(MSYS)
    set(SFML_STATIC_LIBRARIES True)
    set(SFML_DIR "/mingw32/lib/cmake/SFML")
else()
    message(FATAL_ERROR "Not supported")
endif()

# Add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(dod
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)

# Add ImGui-SFML
find_package(ImGui-SFML REQUIRED)
if(NOT ImGui-SFML_FOUND)
    message(FATAL_ERROR "ImGui-SFML not found")
endif()
target_link_libraries(dod
    ImGui-SFML::ImGui-SFML
)

# Add OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif()
target_include_directories(dod
    PUBLIC ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(dod
    ${OPENGL_LIBRARIES}
)

# Add fmt
find_package(fmt REQUIRED)
if(NOT fmt_FOUND)
    message(FATAL_ERROR "fmt not found")
endif()
target_link_libraries(dod
    fmt::fmt
)

# Add nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(dod
    nlohmann_json::nlohmann_json
)

# Add nativefiledialog
target_link_libraries(dod
    nfd::nfd
)

if(MSYS)
    target_link_libraries(dod
        -static-libgcc -static-libstdc++ -lwsock32 -lws2_32
        -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
        # -mconsole / -mwindows
    )
endif()

# Copy files
# file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
# file(GLOB BINARY_DEP_DLLS
#     "${CMAKE_SOURCE_DIR}/bin/*.dll"
# )
# file(COPY ${BINARY_DEP_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
