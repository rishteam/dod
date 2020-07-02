#!/bin/bash

# BINARY_NAME=Sandbox/Sandbox.exe
BINARY_NAME=Editor/Editor.exe

usage() {
    cat <<EOF
Usage: $0 <command>
    full [run]     Remove the whole build/* before cmake
    clean [run]    Call \`make clean\` after cmake before \`make -j6\`
    run            Run the program after building
    docs [open]    Build the docs
    cloc           Calculate Lines Of Code
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

# -----------------------------------------------------------------------------

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
    exit 1
fi

# Create build dir if it is not exist
[[ -d ./build ]] || mkdir build;

pushd build > /dev/null

case "$1" in

"full")
    rm -rf ./*
    cmake_go
    make_go
    [[ "$2" == "run" ]] && run $?
;;

"clean")
    make clean
    make_go
    [[ "$2" == "run" ]] && run $?
;;

"run")
    cmake_go
    make_go
    run $?
;;

"docs")
    make doxygen-docs
    [[ "$2" == "open" ]] && start_go ./RishEngine/docs/html/index.html
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