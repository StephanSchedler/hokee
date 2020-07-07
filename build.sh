#!/bin/bash

#
# Shell script to build hokee project
# usage: build.sh Release|Debug
#
set -e

echo ==== Update submodules ====
git submodule update --init --recursive

echo ==== Prepare libfmt build directory ====
mkdir -p third_party/build/fmt/$1 
pushd third_party/build/fmt/$1
echo ==== Genereate libfmt build system ====
cmake -DCMAKE_BUILD_TYPE=$1 -DFMT_TEST=FALSE -DCMAKE_INSTALL_PREFIX=. ../../../fmt 
echo ==== Run libfmt build ====
cmake --build . --config $1 --target install --parallel
popd

echo ==== Prepare build directory ====
mkdir -p build
pushd build

echo ==== Generate default build system ====
cmake -DCMAKE_BUILD_TYPE=$1 -DCMAKE_INSTALL_PREFIX=.. ..

echo ==== Run build ====
cmake --build . --config $1 --target install --parallel

popd
