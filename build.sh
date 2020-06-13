#!/bin/bash
set -e

echo ==== Update submodules ====
git submodule update --init --recursive

echo ==== Prepare libfmt build directory ====
mkdir -p third_party/build/fmt/$2/$1 
pushd third_party/build/fmt/$2/$1
echo ==== Genereate libfmt build system ====
cmake -DCMAKE_BUILD_TYPE=$1 -DFMT_TEST=FALSE -DCMAKE_INSTALL_PREFIX=. ../../../../fmt 
echo ==== Run libfmt build ====
cmake --build . --config $1 --target install --parallel
popd

echo ==== Prepare build directory ====
mkdir -p build/$2/$1 
pushd build/$2/$1
echo ==== Try to find ninja build system ====

echo ==== Genereate Ninja build system ====
cmake -DCMAKE_BUILD_TYPE=$1 ../../..

echo ==== Generate default build system ====
cmake -DCMAKE_BUILD_TYPE=$1 ../../..

echo ==== Run build ====
cmake --build . --config $1 --parallel

echo ==== Run tests ====
ctest -C %1 --output-on-failure
popd
