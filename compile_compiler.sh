#!/usr/bin/bash

mkdir -p build/ &> /dev/null

pushd build/ &> /dev/null

# cmake -S ../ -B . && ninja clean && ninja all
cmake -S ../  && make clean && make all

popd &> /dev/null
