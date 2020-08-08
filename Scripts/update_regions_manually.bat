@echo off
REM This script runs from command line, so is in <Project>\Scripts
set PATH=%PATH;..\..\..\Tools\GetRegions
@echo on
REM Modify to specify name of axf file
GetRegions.exe ..\Objects\LCDs_Profiler.axf -c -s  -o..\Source\Profiler\region.c