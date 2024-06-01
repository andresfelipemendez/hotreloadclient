@echo off
REM Build the hotreload executable
call buildscripts\build_hotreload.bat
if errorlevel 1 goto end

REM Build the client library
call buildscripts\build_client_lib.bat
if errorlevel 1 goto end

REM Build the filewatcher
call buildscripts\build_filewatcher.bat
if errorlevel 1 goto end

REM Run the hotreload executable
start build\hotreload.exe

REM Run the filewatcher
start build\filewatcher.exe

:end
pause
