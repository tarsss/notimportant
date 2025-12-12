@echo off
if not exist build mkdir build
pushd build
cl ../src/notimp_win32.c /Fenotimp /nologo /Zi /link user32.lib gdi32.lib opengl32.lib /incremental:no
popd