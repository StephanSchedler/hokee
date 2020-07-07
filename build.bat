REM
REM Shell script to build hokee project and run tests
REM usage: build.bat Release|Debug
REM
@echo off

echo ==== Update submodules ====
git submodule update --init --recursive

echo ==== Prepare libfmt build directory ====
md third_party\build\fmt\%1
pushd third_party\build\fmt\%1
echo ==== Genereate libfmt build system ====
call cmake -DFMT_TEST=FALSE -DCMAKE_CONFIGURATION_TYPES=%1 -DCMAKE_BUILD_TYPE=%1 -DCMAKE_INSTALL_PREFIX=. ..\..\..\fmt
if %errorlevel% neq 0 exit /b %errorlevel%
echo ==== Run libfmt build ====
call cmake --build . --config %1 --target install --parallel
if %errorlevel% neq 0 exit /b %errorlevel%
popd

echo ==== Prepare build directory ====
md build
pushd build
echo ==== Try to find ninja build system ====
where ninja
if %errorlevel% equ 0 (
    echo ==== Genereate Ninja build system ====
    call cmake -G Ninja -DCMAKE_BUILD_TYPE=%1 -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_INSTALL_PREFIX=.. .. 
    if %errorlevel% neq 0 exit /b %errorlevel%
) else (
    echo ==== Generate default build system ====
    call cmake -DCMAKE_BUILD_TYPE=%1 -DCMAKE_INSTALL_PREFIX=.. ..
)

echo ==== Run build ====
rm -r ../bin/* ../test_data/*
call cmake --build . --config %1 --target install --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

echo ==== Run test(s) ====
call ctest -C %1 --output-on-failure 
if %errorlevel% neq 0 exit /b %errorlevel%

popd
