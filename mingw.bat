@ECHO off

SET PATH=%PATH%;C:\Program Files\CMake\bin
SET PATH=%PATH%;C:\Program Files\Ninja
SET PATH=%PATH%;C:\TDM-GCC-64\bin

SET CONFIG=%1
IF [%CONFIG%] == [] SET CONFIG=Release

CALL %~dp0build.bat %CONFIG% GNU