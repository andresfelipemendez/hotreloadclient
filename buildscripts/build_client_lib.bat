@echo off
setlocal
REM Create build directory if it doesn't exist
if not exist ..\build mkdir ..\build

set GLFW_LIB_PATH=lib\glfw\lib-vc2022
cl /EHsc /LD /Fe:build\client_lib.dll ^
/std:c++17 ^
/Ilib\glad\include ^
/Ilib\glfw\include ^
/Ilib\entt\include ^
src\client\client.cpp ^
lib\glad\src\glad.c ^
lib\glfw\lib-vc2022\glfw3.lib ^
opengl32.lib ^
gdi32.lib ^
user32.lib ^
shell32.lib ^
/link ^
/MACHINE:X64 ^
/NODEFAULTLIB:libcmt.lib

if errorlevel 1 goto end
:end
exit /b %errorlevel%
