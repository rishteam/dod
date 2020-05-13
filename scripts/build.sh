#!/bin/bash

usage() {
    cat <<EOF
Usage: $0 <command>
    full [run]     Remove the whole build/* before cmake
    clean [run]    Call \`make clean\` after cmake before \`make -j6\`
    run            Run the program after building

EOF
}

if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
    exit 1
fi


if [[ "$1" == "full" ]]; then
    rm -rf ./build/*
fi

pushd build > /dev/null

cmake -G "MSYS Makefiles" ..

if [[ "$1" == "clean" ]]; then
    make clean
fi

make -j6

if [[ "$1" == "run" || "$2" == "run" ]];then
    ./dod
fi

popd > /dev/null