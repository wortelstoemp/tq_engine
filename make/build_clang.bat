@echo off

mkdir ..\bin
xcopy /D "..\deps\libs\*.dll" "..\bin\"

clang ../code/main.c -std=c89 -pedantic -O2 -g2 -gdwarf-2 -Wall -fstack-protector ..\deps\libs\glew32.lib ..\deps\libs\SDL2.lib ..\deps\libs\SDL2main.lib -o ../bin/main.exe