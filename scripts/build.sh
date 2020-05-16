#!/bin/bash

BINARY_NAME=dod

usage() {
    cat <<EOF
Usage: $0 <command>
    full [run]     Remove the whole build/* before cmake
    clean [run]    Call \`make clean\` after cmake before \`make -j6\`
    run            Run the program after building
    docs [open]    Build the docs
EOF
}

run() {
    [[ $? -eq 0 ]] && ./$BINARY_NAME
}

cmake_go() {
    cmake -G "MSYS Makefiles" ..
}

make_go() {
    make -j6
}

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
    make_go
    run $?
;;

"docs")
    make doxygen-docs
    [[ "$2" == "open" ]] && start ./docs/html/index.html
;;

"-h" | "--help")
    usage
;;

*)
    usage
;;

esac

popd > /dev/null