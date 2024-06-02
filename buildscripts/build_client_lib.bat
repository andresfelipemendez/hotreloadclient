@echo off
setlocal
REM Create build directory if it doesn't exist
if not exist ..\build mkdir ..\build

set GLFW_LIB_PATH=lib\glfw\lib-vc2022
cl /EHsc /LD /Fe:build\client_lib.dll ^
/std:c++17 ^
/Ilib\imgui ^
/Ilib\glad\include ^
/Ilib\glfw\include ^
/Ilib\entt\include ^
src\client\client.cpp ^
lib\glad\src\glad.c ^
lib\imgui\imgui.cpp ^
lib\imgui\imgui_impl_glfw.cpp ^
lib\imgui\imgui_impl_opengl3.cpp ^
lib\imgui\imgui_demo.cpp ^
lib\imgui\imgui_draw.cpp ^
lib\imgui\imgui_widgets.cpp ^
lib\imgui\imgui_tables.cpp ^
lib\glfw\lib-vc2022\glfw3.lib ^
opengl32.lib ^
gdi32.lib ^
user32.lib ^
shell32.lib ^
/Zi /Fd:build\client_lib.pdb ^
/link ^
/MACHINE:X64 ^
/NODEFAULTLIB:libcmt.lib ^
/DEBUG

if errorlevel 1 goto end
:end
exit /b %errorlevel%
