#!/bin/bash

CONFIG=$1
[[ -z $CONFIG ]] && CONFIG=Release

[[ ! -d out/$CONFIG ]] && mkdir -p out/$CONFIG
cmake -G Ninja . -Bout/$CONFIG -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCI_BUILD=1 -DCMAKE_BUILD_TYPE=$CONFIG
ninja -v -C out/$CONFIG