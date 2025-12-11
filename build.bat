@echo off
if not exist build mkdir build
pushd build
cl ../src/notimp_win32.c /Fenotimp /nologo /Zi /link user32.lib
popd