@echo off
cl /EHsc /Fe:build\filewatcher.exe src\filewatcher\filewatcher.cpp ws2_32.lib /std:c++17
if errorlevel 1 goto end
:end
