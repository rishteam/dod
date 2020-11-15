#!/bin/bash

usage() {
    cat <<EOF
Usage: $0 <command>
    full [target]        Remove the whole build/* before cmake
    clean [target]       Call \`make clean\` after cmake before \`make -j6\`
    cmake                Rebuild the cmake files only
    run                  Run the program after building
    docs [engine|editor] Build the docs Remember to specify the target
    cloc                 Calculate Lines Of Code

Targets:
    sandbox - RishEngine Sandbox
    editor  - Rish Editor
EOF
}

run() {
    [[ ! $? -eq 0 ]] && return
    if [[ "`uname`" == "Darwin" ]]; then
        ./`echo "$BINARY_NAME" | sed 's/.exe//'`
    else
        ./$BINARY_NAME
    fi
}

cmake_go() {
    if [[ "`uname`" == "Darwin" ]]; then
        cmake ..
    else
        cmake -G "MSYS Makefiles" ..
        cmake -S../RishEditor/docs -BRishEditorDocs -G "MSYS Makefiles"
    fi
}

make_go() {
    make -j6
}

start_go() {
    if [[ "`uname`" == "Darwin" ]]; then
        open "$1"
    else
        start "$1"
    fi
}

target_go() {
    if [[ "$1" == "sandbox" ]]; then
        start_go Sandbox/Sandbox.exe
    elif [[ "$1" == "editor" ]]; then
        start_go RishEditor/Editor.exe
    fi
}

# -----------------------------------------------------------------------------

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
    exit 1
fi

# Create build dir if it is not exist
[[ -d ./build ]] || mkdir build;

pushd build > /dev/null

case "$1" in

"cmake")
    rm -rf ./*
    cmake_go
;;

"full")
    rm -rf ./*
    cmake_go
    make_go
    [[ "$2" != "" ]] && target_go "$2"
;;

"clean")
    make clean
    make_go
    [[ "$2" != "" ]] && target_go "$2"
;;

"run")
    cmake_go
    make_go
    run $?
;;

"docs")
    cmake_go

    # Build RishEngine docs
    make doxygen-docs

    # Build RishEditor docs
    pushd RishEditorDocs
    make doxygen-docs
    popd

    #
    [[ "$2" == "engine" ]] && start_go ./RishEngine/docs/html/index.html
    [[ "$2" == "editor" ]] && start_go ./RishEditorDocs/html/index.html
;;

"cloc")
    cloc --exclude-dir=deps,build,cmake-build-debug,cmake-build-release,.idea,.vscode ..
;;

"-h" | "--help")
    usage
;;

*)
    usage
;;

esac

popd > /dev/null