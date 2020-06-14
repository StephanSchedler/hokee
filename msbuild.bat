@ECHO off

PUSHD
CALL "C:/Program Files (x86)/Microsoft Visual Studio/2019/BuildTools/VC/Auxiliary/Build/vcvarsall.bat" x64 
POPD

SET CONFIG=%1
IF [%CONFIG%] == [] SET CONFIG=Release

CALL %~dp0build.bat %CONFIG%