#!/bin/bash

#
# Shell script to build hokee project
# usage: build.sh Release|Debug
#
set -ex

if [ -z "$var" ]
then
      TARGET=Release
else
      TARGET=$1
fi
echo TARGET=$TARGET

echo ==== Update submodules ====
git submodule update --init --recursive

echo ==== Prepare libfmt build directory ====
mkdir -p third_party/build/fmt/$TARGET 
pushd third_party/build/fmt/$TARGET
echo ==== Genereate libfmt build system ====
cmake -DCMAKE_BUILD_TYPE=$TARGET -DFMT_TEST=FALSE -DCMAKE_INSTALL_PREFIX=. ../../../fmt 
echo ==== Run libfmt build ====
cmake --build . --config $TARGET --target install
popd

echo ==== Prepare build directory ====
mkdir -p build
pushd build

echo ==== Generate default build system ====
NINJA_BIN=$(which ninja) || echo "Could not find ninja. Build with default..."
CMAKE_OPTIONS=
if [ -x "$NINJA_BIN" ] ; then
      CMAKE_OPTIONS="-G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
fi
cmake $CMAKE_OPTIONS -DCMAKE_BUILD_TYPE=$TARGET -DCMAKE_INSTALL_PREFIX=.. ..


echo ==== Run build ====
cmake --build . --config $TARGET --target install

popd
