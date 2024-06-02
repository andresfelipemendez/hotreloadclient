@echo off
setlocal
REM Create build directory if it doesn't exist
if not exist ..\build mkdir ..\build

cl /EHsc /LD /Fe:build\client_lib.dll /Fo..\build\ src\client\client.cpp
if errorlevel 1 goto end
:end
exit /b %errorlevel%
