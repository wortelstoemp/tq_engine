@echo off

set DEBUGVARS=/Od /Zi /Za /FC /W4 /wd4100 /wd4001 /nologo /Tc
set RELEASEVARS=/O2 /Oi /FC /W4 /wd4100 /nologo /Tc

set code=..\code\main.c

set includes=/I "..\deps\includes"

set libs=/link opengl32.lib^
		/LIBPATH:..\deps\libs\ SDL2.lib SDL2main.lib glew32.lib

set options=%DEBUGVARS% %code% %includes% %libs%

mkdir ..\bin
xcopy /D "..\deps\libs\*.dll" "..\bin\"

pushd ..\bin
cl -EHsc %options% /SUBSYSTEM:CONSOLE
popd