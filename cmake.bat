@echo off
REM
REM Shell script to build call cmake
REM usage: build.bat Release|Debug
REM

if exist "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
    SET VCVARS_BAT="C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvarsall.bat"
) 
if exist "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" (
    SET VCVARS_BAT="C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build/vcvarsall.bat"
)
if exist "C:/Program Files (x86)/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
    SET VCVARS_BAT="C:/Program Files (x86)/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat"
) 
if exist "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" (
    SET VCVARS_BAT="C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat"
)

IF [%VCVARS_BAT%] == [] (
    echo ERROR: Could not find vcvarsall.bat!
    exit /b -1 
)

pushd
call %VCVARS_BAT% x64
if %errorlevel% neq 0 exit /b %errorlevel%
popd

pushd build

rm -r ../bin/* ../test_data/*
call cmake --build . --config %1 --target install --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

popd
