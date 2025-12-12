@echo off
if not exist build mkdir build
pushd build
set warnings=-Wno-incompatible-pointer-types-discards-qualifiers -Wno-pointer-sign
set flags=/Zi /Fenotimp /nologo
set link=user32.lib gdi32.lib opengl32.lib /incremental:no
clang-cl ../src/notimp_win32.c %flags% %warnings% /link %link%
popd