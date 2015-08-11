
@echo off
pushd ..\build
REM SDL2.lib SDL2main.lib
set imports=opengl32.lib SDL2.lib
REM /link /SUBSYSTEM:WINDOWS
REM -MTd
REM -SUBSYSTEM:WINDOWS
cl -Zi -Od -MD ../pacman/pacman.c %imports% -link -SUBSYSTEM:WINDOWS
