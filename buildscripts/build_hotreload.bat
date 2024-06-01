@echo off
setlocal
cl /EHsc /Fe:build\hotreload.exe src\client\hotreload.cpp ws2_32.lib