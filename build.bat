@echo off

git submodule update --init --recursive

echo third_party\build\fmt\%2\%1
if not exist "third_party\build\fmt\%2\%1" (
    md third_party\build\fmt\%2\%1

    pushd third_party\build\fmt\%2\%1

    cmake -G Ninja -DFMT_TEST=FALSE -DCMAKE_BUILD_TYPE=%1 -DCMAKE_INSTALL_PREFIX=. ..\..\..\..\fmt
    if %errorlevel% neq 0 exit /b %errorlevel%

    ninja -j8 install
    if %errorlevel% neq 0 exit /b %errorlevel%

    popd
)

if not exist "build\%2\%1\build.ninja" (
    md build\%2\%1    
    pushd build\%2\%1
    cmake -G Ninja -DCMAKE_BUILD_TYPE=%1 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_INSTALL_PREFIX=..\..\.. ..\..\.. 
    if %errorlevel% neq 0 exit /b %errorlevel%
    popd
)

cd build\%2\%1
ninja -j8 install
if %errorlevel% neq 0 exit /b %errorlevel%

ctest --output-on-failure
if %errorlevel% neq 0 exit /b %errorlevel%
