@echo off
REM
REM Shell script to build call cmake
REM usage: build.bat Release|Debug
REM

pushd
if exist "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" (
    call "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" x64 
) else (
    if exist "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
        call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/VC/Auxiliary/Build/vcvarsall.bat" x64 
    ) else (
        echo ERROR: Could not find vcvarsall.bat!
        exit /b -1 
    ) 
)
popd

pushd build

rm -r ../bin/* ../test_data/*
call cmake --build . --config %1 --target install --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

popd
