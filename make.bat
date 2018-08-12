@echo off
setlocal

:: use most recent VS version found
reg query HKEY_CLASSES_ROOT\VisualStudio.DTE.12.0 >nul 2>nul
IF %errorlevel%==0 set GENERATOR=Visual Studio 12
reg query HKEY_CLASSES_ROOT\VisualStudio.DTE.14.0 >nul 2>nul
IF %errorlevel%==0 set GENERATOR=Visual Studio 14
reg query HKEY_CLASSES_ROOT\VisualStudio.DTE.15.0 >nul 2>nul
IF %errorlevel%==0 set GENERATOR=Visual Studio 15

:: default to 64bit on 64bit arch
if not defined platform reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set platform=x86 || set platform=x64
if /I "%platform%" EQU "x64" (set GENERATOR=%GENERATOR% Win64)

IF NOT "x%1" == "x" GOTO :%1

GOTO :build

:projects
cmake -H. -Bbuild -G"%GENERATOR%"
GOTO :end

:build
cmake -H. -Bbuild -G"%GENERATOR%"
cmake --build build --config Release
GOTO :end

:clean
IF EXIST build RMDIR /S /Q build
GOTO :end

:end