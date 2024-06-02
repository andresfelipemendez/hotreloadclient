@echo off
if not exist ..\build mkdir ..\build
cl /EHsc /Fe:build\hotreload.exe /Fo..\build\ /Ilib\entt\include src\hotreload\hotreload.cpp src\hotreload\hotreload_windows.cpp ws2_32.lib /std:c++17 /DEBUG /Zi
if errorlevel 1 goto end
:end
exit /b %errorlevel%